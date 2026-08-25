#ifndef Common_h
#define Common_h

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <FFat.h>
#include <time.h>
#include "logger.h"
#include "led.h"
#include "oled.h"

// First, re-define WEBSOCKETS_MAX_DATA_SIZE to (150 * 1024) in WebSocketsClient.h

// Global variables
#define VERSION "2.0.1"

#define APPID ""
#define TOKEN ""

// Constants from Python code
#define PROTOCOL_VERSION 0x01
#define DEFAULT_HEADER_SIZE 0x01

// Message Types
#define CLIENT_FULL_REQUEST 0x01
#define CLIENT_AUDIO_ONLY_REQUEST 0x02
#define SERVER_FULL_RESPONSE 0x09
#define SERVER_ACK 0x0B
#define SERVER_ERROR_RESPONSE 0x0F

// Message Type Specific Flags
#define NO_SEQUENCE 0x00
#define POS_SEQUENCE 0x01
#define NEG_SEQUENCE 0x02
#define NEG_SEQUENCE_1 0x03

// Message Serialization
#define NO_SERIALIZATION 0x00
#define JSON 0x01
#define THRIFT 0x03
#define CUSTOM_TYPE 0x0F

// Message Compression
#define NO_COMPRESSION 0x00
#define GZIP 0x01
#define CUSTOM_COMPRESSION 0x0F

// Global variables
extern unsigned long last_time;
extern bool enable_act;
extern bool start_chat;

// Functions
void setup_FFat();
String get_mac_address();
String generate_uuid();
String get_byte_str(uint8_t* payload, size_t length);
String get_local_ip();

#endif