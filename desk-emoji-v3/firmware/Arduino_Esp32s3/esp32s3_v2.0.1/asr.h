#ifndef ASR_H
#define ASR_H

#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "common.h"
#include "audio.h"
#include "act.h"

#define MAX_WAIT_TIME 10
#define RECORD_TIME 10
#define MAX_SILENCE_TIME 2
#define BUFFER_SIZE (8 * 1024)

class AsrClient {

public:
    String asrResult();
    AsrClient();
    void connect();
    void disconnect();
    void loop(int delay_time = 100);
    bool ASR();

private:
    const char* host = "openspeech.bytedance.com";
    const char* asr_url = "/api/v2/asr";
    const char* appid = APPID;
    const char* token = TOKEN;
    const char* cluster = "volcengine_streaming_common";
    const char* audio_format = "raw";
    const int sample_rate = 16000;
    const int bits = 16;
    const int channel = 1;
    const int success_code = 1000;
    const char* language = "zh-CN";
    const char* uid = "388808087185088";
    const char* workflow = "audio_in,resample,partition,vad,fe,decode,itn,nlu_punctuate";
    bool messageReceived = false;
    bool lastMessageReceived = false;
    bool voice_detected = false;
    String asr_result = "";

    WebSocketsClient webSocket;

    void handleWebSocketEvent(WStype_t type, uint8_t * payload, size_t length);
    bool waitForMessage();
    void blink_loop(uint32_t color = COLOR_BLUE);
    void generateHeader(uint8_t* header,
                        uint8_t version = PROTOCOL_VERSION,
                        uint8_t messageType = CLIENT_FULL_REQUEST,
                        uint8_t messageTypeSpecificFlags = NO_SEQUENCE,
                        uint8_t serialMethod = JSON,
                        uint8_t compressionType = NO_COMPRESSION,
                        uint8_t reservedData = 0x00);
    String constructRequest();
    bool sendFullRequest();
    bool sendAudioRequest(uint8_t *data, size_t length, bool isLast);
    String parseResponse(uint8_t* payload, size_t length);
    String parseJson(String jsonString);
};

#endif 