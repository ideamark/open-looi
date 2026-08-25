#ifndef Connect_h
#define Connect_h

#include <queue>
#include <ArduinoJson.h>
#include "emoji.h"
#include "head.h"

#define SERVICE_UUID "4db9a22d-6db4-d9fe-4d93-38e350abdc3c"
#define CHARACTERISTIC_UUID "ff1cdaef-0105-e4fb-7be2-018500c2e927"
extern std::queue<String> commandQueue;

void handle_cmd();
void executeCommand(String cmd);
void executeFactoryCommand(String cmd);

#endif