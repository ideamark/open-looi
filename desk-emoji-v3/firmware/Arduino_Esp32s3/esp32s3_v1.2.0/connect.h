#ifndef Connect_h
#define Connect_h

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <queue>
#include <ArduinoJson.h>
#include "emoji.h"
#include "head.h"
#include "animation.h"

#define SERVICE_UUID "4db9a22d-6db4-d9fe-4d93-38e350abdc3c"
#define CHARACTERISTIC_UUID "ff1cdaef-0105-e4fb-7be2-018500c2e927"
extern std::queue<String> commandQueue;

class BltCallbacks : public BLECharacteristicCallbacks {  
public:  
    void onWrite(BLECharacteristic* pCharacteristic) override;
};

void setup_BLE();
void handle_cmd();
void executeCommand(String cmd);
void executeFactoryCommand(String cmd);

#endif