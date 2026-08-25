#include "asr.h"


AsrClient::AsrClient() {
}

void AsrClient::connect() {
    if (!webSocket.isConnected()) {
        webSocket.beginSSL(host, 443, asr_url);
        webSocket.setExtraHeaders(("Authorization: Bearer; " + String(token)).c_str());
        
        webSocket.onEvent([this](WStype_t type, uint8_t* payload, size_t length) {
            this->handleWebSocketEvent(type, payload, length);
        });

        // Wait until connected
        unsigned long startTime = millis();
        while (!webSocket.isConnected() && millis() - startTime < 5000) {
            blink_loop();
        }
    }
}

void AsrClient::disconnect() {
    if (webSocket.isConnected()) {
        webSocket.disconnect();
        // Wait until disconnected
        unsigned long startTime = millis();
        while (webSocket.isConnected() && millis() - startTime < 3000) {
            loop();
        }
    }
}

void AsrClient::handleWebSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
    String result;
    switch(type) {
        case WStype_CONNECTED:
            log_info("ASR webSocket connected");
            break;
        case WStype_DISCONNECTED:
            log_info("ASR webSocket disconnected");
            if (!lastMessageReceived) {
                set_led(COLOR_BLACK);
            }
            break;
        case WStype_BIN:
            result = parseResponse(payload, length);
            log_debug("%s", result.c_str());
            messageReceived = true;
            if (lastMessageReceived) {
                asr_result = parseJson(result);
            }
            webSocket.loop();
            break;
        case WStype_ERROR:
            log_error("ASR webSocket Error.");
            break;
        case WStype_PING:
            log_debug("ASR Ping.");
            break;
        case WStype_PONG:
            log_debug("ASR Pong.");
            break;
    }
}

void AsrClient::loop(int delay_time) {
    if (webSocket.isConnected()) {
        webSocket.loop();
        delay(delay_time);
    }
}

void AsrClient::blink_loop(uint32_t color) {
    webSocket.loop();
    blink_led(color);
}

bool AsrClient::waitForMessage() {
    unsigned long waitStartTime = millis();
    while (!messageReceived) {
        loop();
        if (millis() - waitStartTime > 3000) {
            log_error("ASR message wait timeout");
            set_led(COLOR_BLACK);
            return false;
        }
    }
    messageReceived = false;
    return true;
}

void AsrClient::generateHeader(uint8_t* header,
                               uint8_t version,
                               uint8_t messageType,
                               uint8_t messageTypeSpecificFlags,
                               uint8_t serialMethod,
                               uint8_t compressionType,
                               uint8_t reservedData) {
    header[0] = (version << 4) | DEFAULT_HEADER_SIZE;
    header[1] = (messageType << 4) | messageTypeSpecificFlags;
    header[2] = (serialMethod << 4) | compressionType;
    header[3] = reservedData;
}

String AsrClient::constructRequest() {
    StaticJsonDocument<1024> doc;
    
    JsonObject app = doc["app"].to<JsonObject>();
    app["appid"] = appid;
    app["cluster"] = cluster;
    app["token"] = token;

    JsonObject user = doc["user"].to<JsonObject>();
    user["uid"] = uid;

    JsonObject audio = doc["audio"].to<JsonObject>();
    audio["format"] = audio_format;
    audio["rate"] = sample_rate;
    audio["language"] = language;
    audio["bits"] = bits;
    audio["channel"] = channel;
    audio["codec"] = "raw";

    JsonObject request = doc["request"].to<JsonObject>();
    request["reqid"] = generate_uuid();
    request["nbest"] = 1;
    request["workflow"] = workflow;
    request["show_language"] = false;
    request["show_utterances"] = false;
    request["result_type"] = "full";
    request["sequence"] = 1;

    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}

bool AsrClient::sendFullRequest() {
    String jsonString = constructRequest();
    uint8_t* payloadStr = (uint8_t*)jsonString.c_str();

    // Calculate total size needed
    size_t payloadLength = strlen((char*)payloadStr);
    size_t totalLength = 4 + 4 + payloadLength;  // header + payload size + json
    
    // Construct full request
    uint8_t* fullRequest = new uint8_t[totalLength];
    uint8_t header[4];
    generateHeader(header);
    memcpy(fullRequest, header, 4);
    fullRequest[4] = (payloadLength >> 24) & 0xFF;
    fullRequest[5] = (payloadLength >> 16) & 0xFF;
    fullRequest[6] = (payloadLength >> 8) & 0xFF;
    fullRequest[7] = payloadLength & 0xFF;
    memcpy(fullRequest + 8, payloadStr, payloadLength);

    log_debug("--------------------------- send ASR full request ---------------------------");
    log_debug("ASR full header bytes: %s", get_byte_str(fullRequest, 8).c_str());
    log_debug("ASR payload string: %s", jsonString.c_str());

    if (!webSocket.sendBIN(fullRequest, totalLength)) {
        delete[] fullRequest;
        return false;
    } else {
        delete[] fullRequest;
        loop();
        return true;
    }
}

bool AsrClient::sendAudioRequest(uint8_t *data, size_t length, bool isLast) {
    // Construct audio request
    uint8_t header[4];
    generateHeader(header, 
                    PROTOCOL_VERSION,
                    CLIENT_AUDIO_ONLY_REQUEST,
                    isLast ? NEG_SEQUENCE : NO_SEQUENCE);

    uint8_t* audioRequest = new uint8_t[8 + length];
    memcpy(audioRequest, header, 4);

    audioRequest[4] = (length >> 24) & 0xFF;
    audioRequest[5] = (length >> 16) & 0xFF;
    audioRequest[6] = (length >> 8) & 0xFF;
    audioRequest[7] = length & 0xFF;

    memcpy(audioRequest + 8, data, length);

    log_debug("--------------------------- send ASR audio request ---------------------------");
    log_debug("ASR audio header bytes: %s", get_byte_str(audioRequest, 8).c_str());

    if (!webSocket.sendBIN(audioRequest, 8 + length)) {
        delete[] audioRequest;
        return false;
    } else {
        delete[] audioRequest;
        loop();
        return true;
    }
}

bool AsrClient::ASR() {
    asr_result = "";
    set_led(COLOR_RED, 10);

    if (!sendFullRequest()) {
        log_error("Send full request Failed!");
        return false;
    }
    if (!waitForMessage()) {
        return false;
    }

    const size_t samples_needed = RECORD_TIME * SAMPLE_RATE;
    size_t samples_recorded = 0;
    int16_t* buffer = new int16_t[BUFFER_SIZE];
    unsigned long silenceStartTime = 0;
    bool isSilent = false;
    voice_detected = false;

    // static unsigned long last_smile_time = 0;
    while (samples_recorded < samples_needed) {
        // if (millis() - last_smile_time > 2000) {
        //     async_random_smile_act();
        //     last_smile_time = millis();
        // }
        size_t samples_to_read = std::min((size_t)BUFFER_SIZE, samples_needed - samples_recorded);
        record(buffer, samples_to_read);
        enhanceVoice(buffer, samples_to_read);

        if (samples_to_read < BUFFER_SIZE) {
            lastMessageReceived = true;
        } else {
            lastMessageReceived = false;
        }

        size_t mean = calculate_mean(buffer, samples_to_read);
        if (mean > SOUND_THRESHOLD) {
            set_led(COLOR_RED, 100);
            isSilent = false;
            silenceStartTime = 0;
        } else {
            set_led(COLOR_RED, 10);
            if (!isSilent) {
                isSilent = true;
                silenceStartTime = millis();
            } else if ((!voice_detected && samples_recorded > SAMPLE_RATE * 3) || samples_recorded > SAMPLE_RATE * MAX_WAIT_TIME) {
                lastMessageReceived = true;
            }
        }

        if (!sendAudioRequest((uint8_t*)buffer, samples_to_read * sizeof(int16_t), lastMessageReceived)) {
            log_error("Send audio request Failed!");
            return false;
        }
        if (!waitForMessage()) {
            return false;
        }
        samples_recorded += samples_to_read;
        if (lastMessageReceived) {
            break;
        }
    }

    delete[] buffer;
    return true;
}

String AsrClient::parseResponse(uint8_t* payload, size_t length) {
    uint8_t protocol_version = payload[0] >> 4;
    uint8_t header_size = payload[0] & 0x0F;
    uint8_t message_type = payload[1] >> 4;
    uint8_t message_type_specific_flags = payload[1] & 0x0F;
    uint8_t serialization_method = payload[2] >> 4;
    uint8_t reserved = payload[3];
    uint8_t* header_extensions = payload + 4;
    uint8_t* payload_data = payload + (header_size * 4);

    String payload_msg = "";

    if (message_type == SERVER_FULL_RESPONSE) {
        payload_msg = String((char*)(payload_data + 4));

    } else if (message_type == SERVER_ACK || message_type == SERVER_ERROR_RESPONSE) {
        payload_msg = String((char*)(payload_data + 8));
        // Check if payload_msg contains "result" key and set voice_detected flag
        if (payload_msg.indexOf("\"result\"") != -1) {
            voice_detected = true;
        }
    } else {
        log_error("undefined message type: 0x%X", message_type);
    }
    return payload_msg;
}

String AsrClient::parseJson(String jsonString) {
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, jsonString);
    if (!error || doc.containsKey("result")) {
        JsonArray results = doc["result"];
        if (results.size() > 0) {
            JsonObject result = results[0];
            if (result.containsKey("text")) {
                String text = result["text"].as<String>();
                return text;
            }
        }
    }
    return "";
}

String AsrClient::asrResult() {
    return asr_result;
}
