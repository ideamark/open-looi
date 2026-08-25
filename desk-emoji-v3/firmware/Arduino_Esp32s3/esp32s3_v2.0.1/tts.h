#ifndef TTS_H
#define TTS_H

#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "common.h"
#include "audio.h"
#include "act.h"

class TtsClient {

public:
    TtsClient();
    void connect();
    void disconnect();
    void loop(int delay_time = 100);
    bool TTS(String text);
    
private:
    const char* host = "openspeech.bytedance.com";
    const char* tts_url = "/api/v1/tts/ws_binary";
    const char* appid = APPID;
    const char* token = TOKEN;
    const char* cluster = "volcano_tts";
    const char* operation = "submit";
    // const char* voice_type = "BV001_streaming";  // 通用女声
    // const char* voice_type = "BV019_streaming";  // 重庆小伙
    // const char* voice_type = "BV701_streaming";  // 擎苍
    // const char* voice_type = "BV504_streaming";  // 英语Jackson
    const char* voice_type = "BV213_streaming";  // 广西表哥
    // const char* voice_type = "BV423_streaming";  // 重庆幺妹

    bool messageReceived = false;
    bool lastMessageReceived = false;
    float volumn = 0.3;
    WebSocketsClient webSocket;

    void handleWebSocketEvent(WStype_t type, uint8_t * payload, size_t length);
    bool waitForMessage();
    void blink_loop(uint32_t color = COLOR_BLUE);
    void generateHeader(uint8_t* header, 
                        uint8_t version = PROTOCOL_VERSION,
                        uint8_t message_type = CLIENT_FULL_REQUEST,
                        uint8_t message_type_specific_flags = NO_SEQUENCE,
                        uint8_t serial_method = JSON,
                        uint8_t compression_type = NO_COMPRESSION,
                        uint8_t reserved_data = 0x00);
    bool sendTtsRequest(String text);
    void parseResponse(uint8_t* payload, size_t length);
};

#endif 