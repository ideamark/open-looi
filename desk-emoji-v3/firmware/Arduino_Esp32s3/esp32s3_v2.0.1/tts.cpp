#include "tts.h"


TtsClient::TtsClient() {
}

void TtsClient::connect() {
    if (!webSocket.isConnected()) {
        webSocket.beginSSL(host, 443, tts_url);
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

void TtsClient::disconnect() {
    if (webSocket.isConnected()) {
        webSocket.disconnect();
        // Wait until disconnected
        unsigned long startTime = millis();
        while (webSocket.isConnected() && millis() - startTime < 3000) {
            loop();
        }
    }
}

void TtsClient::handleWebSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_CONNECTED:
            log_info("TTS webSocket connected");
            break;
        case WStype_DISCONNECTED:
            log_info("TTS webSocket disconnected");
            if (!lastMessageReceived) {
                set_led(COLOR_BLACK);
            }
            stop_play();
            break;
        case WStype_BIN:
            set_led(COLOR_GREEN, 50);
            parseResponse(payload, length);
            messageReceived = true;
            set_led(COLOR_GREEN, 5);
            webSocket.loop();
            break;
        case WStype_ERROR:
            log_error("TTS webSocket Error.");
            stop_play();
            break;
        case WStype_PING:
            log_debug("TTS Ping.");
            break;
        case WStype_PONG:
            log_debug("TTS Pong.");
            break;
    }
}

void TtsClient::loop(int delay_time) {
    if (webSocket.isConnected()) {
        webSocket.loop();
        delay(delay_time);
    }
}

void TtsClient::blink_loop(uint32_t color) {
    webSocket.loop();
    blink_led(color);
}

bool TtsClient::waitForMessage() {
    unsigned long waitStartTime = millis();
    while (!messageReceived) {
        loop();
        if (millis() - waitStartTime > 3000) {
            log_error("TTS message wait timeout");
            set_led(COLOR_BLACK);
            return false;
        }
    }
    messageReceived = false;
    return true;
}

void TtsClient::generateHeader(uint8_t* header,
                               uint8_t version,
                               uint8_t message_type,
                               uint8_t message_type_specific_flags,
                               uint8_t serial_method,
                               uint8_t compression_type,
                               uint8_t reserved_data) {
    header[0] = (version << 4) | DEFAULT_HEADER_SIZE;
    header[1] = (message_type << 4) | message_type_specific_flags;
    header[2] = (serial_method << 4) | compression_type;
    header[3] = reserved_data;
}

bool TtsClient::sendTtsRequest(String text) {
    StaticJsonDocument<1024> doc;
    
    // Construct JSON request
    JsonObject app = doc["app"].to<JsonObject>();
    app["appid"] = appid;
    app["token"] = token;
    app["cluster"] = cluster;

    JsonObject user = doc["user"].to<JsonObject>();
    user["uid"] = "388808087185088";

    JsonObject audio = doc["audio"].to<JsonObject>();
    audio["voice_type"] = voice_type;
    audio["encoding"] = "pcm";
    audio["rate"] = 16000;
    audio["speed_ratio"] = 1.0;
    audio["volume_ratio"] = 1.0;
    audio["pitch_ratio"] = 1.0;
    audio["emotion"] = "happy";

    JsonObject request = doc["request"].to<JsonObject>();
    request["reqid"] = generate_uuid();
    request["text"] = text;
    request["text_type"] = "plain";
    request["operation"] = operation;

    String jsonString;
    serializeJson(doc, jsonString);
    uint8_t* payloadStr = (uint8_t*)jsonString.c_str();

    // Calculate total size needed
    size_t payloadLength = strlen((char*)payloadStr);
    size_t totalLength = 4 + 4 + payloadLength; // header + payload size + json

    // Create full request
    uint8_t* fullRequest = new uint8_t[totalLength];
    uint8_t header[4];
    generateHeader(header);
    memcpy(fullRequest, header, 4);
    fullRequest[4] = (payloadLength >> 24) & 0xFF;
    fullRequest[5] = (payloadLength >> 16) & 0xFF;
    fullRequest[6] = (payloadLength >> 8) & 0xFF;
    fullRequest[7] = payloadLength & 0xFF;
    memcpy(fullRequest + 8, payloadStr, payloadLength);

    log_debug("--------------------------- send TTS request ---------------------------");
    log_debug("TTS full header bytes: %s", get_byte_str(fullRequest, 8).c_str());
    log_debug("TTS payload string: %s", jsonString.c_str());

    // Send request
    if (!webSocket.sendBIN(fullRequest, totalLength)) {
        delete[] fullRequest;
        return false;
    } else {
        delete[] fullRequest;
        loop();
        return true;
    }
}

bool TtsClient::TTS(String text) {
    if (text.isEmpty()) {
        log_error("TTS text is empty!");
        lastMessageReceived = false;
        return false;
    }

    if (!sendTtsRequest(text)) {
        log_error("Send TTS request Failed!");
        lastMessageReceived = false;
        return false;
    }

    unsigned long loopStartTime = millis();
    unsigned long lastActionTime = millis();
    while (!lastMessageReceived) {
        if (!waitForMessage()) {
            return false;
        }
        if (millis() - loopStartTime > 6000) {       // start
            if (millis() - lastActionTime > 3000) {  // trigger
                async_speech_act();
                lastActionTime = millis();
            }
        }
    }
    lastMessageReceived = false;
    log_info("TTS END");
    return true;
}

void TtsClient::parseResponse(uint8_t* payload, size_t length) {
    uint8_t protocol_version = payload[0] >> 4;
    uint8_t header_size = payload[0] & 0x0F;
    uint8_t message_type = payload[1] >> 4;
    uint8_t message_type_specific_flags = payload[1] & 0x0F;
    uint8_t serialization_method = payload[2] >> 4;
    uint8_t reserved = payload[3];
    uint8_t* header_extensions = payload + 4;
    uint8_t* payload_data = payload + (header_size * 4);

    log_debug("--------------------------- TTS response ---------------------------");
    log_debug("            Protocol version: 0x%X - version %d", protocol_version, protocol_version);
    
    log_debug("                 Header size: 0x%X - %d bytes", header_size, header_size * 4);
    
    log_debug("                Message type: 0x%X - %s", message_type,
              message_type == SERVER_ACK ? "audio-only server response" :
              message_type == 0x0C ? "frontend server response" :
              message_type == SERVER_ERROR_RESPONSE ? "error message from server" :
              "undefined message type");
    
    log_debug(" Message type specific flags: 0x%X - %s", message_type_specific_flags,
              message_type_specific_flags == NO_SEQUENCE ? "no sequence number" :
              message_type_specific_flags == POS_SEQUENCE ? "sequence number > 0" :
              message_type_specific_flags == NEG_SEQUENCE ? "last message from server (seq < 0)" :
              message_type_specific_flags == NEG_SEQUENCE_1 ? "sequence number < 0" :
              "undefined flag");
    
    log_debug("Message serialization method: 0x%X - %s", serialization_method,
              serialization_method == NO_SERIALIZATION ? "no serialization" :
              serialization_method == JSON ? "JSON" :
              serialization_method == CUSTOM_TYPE ? "custom type" :
              "undefined method");
    
    log_debug("                    Reserved: 0x%X", reserved);
    
    if (header_size != 1) {
        log_debug("           Header extensions: %s", get_byte_str(header_extensions, (header_size-1)*4).c_str());
    }

    if (message_type == SERVER_ACK) {
        if (message_type_specific_flags == NO_SEQUENCE) {
            log_debug("                Payload size: 0");

        } else {
            int32_t sequence_number = (payload_data[0] << 24) | 
                                      (payload_data[1] << 16) | 
                                      (payload_data[2] << 8) | 
                                       payload_data[3];
            
            uint32_t payload_size = (payload_data[4] << 24) |
                                    (payload_data[5] << 16) |
                                    (payload_data[6] << 8) |
                                     payload_data[7];
                                
            log_debug("             Sequence number: %d", sequence_number);
            log_debug("                Payload size: %u bytes", payload_size);

            // play audio
            int16_t* buffer = (int16_t*)(payload_data + 8);
            size_t buffer_size = payload_size / 2;
            play(buffer, buffer_size, volumn);

            if (sequence_number < 0) {
                lastMessageReceived = true;
                delay(200);
                stop_play();
            }
        }

    } else if (message_type == SERVER_ERROR_RESPONSE) {
        uint32_t code = (payload_data[0] << 24) |
                        (payload_data[1] << 16) |
                        (payload_data[2] << 8) |
                         payload_data[3];
                       
        uint32_t msg_size = (payload_data[4] << 24) |
                            (payload_data[5] << 16) |
                            (payload_data[6] << 8) |
                             payload_data[7];

        String error_msg = String((char*)(payload_data + 8));
        
        log_error("          Error message code: %u", code);
        log_error("          Error message size: %u bytes", msg_size);
        log_error("               Error message: %s", error_msg.c_str());

    } else {
        log_error("Undefined message type!");
    }
}
