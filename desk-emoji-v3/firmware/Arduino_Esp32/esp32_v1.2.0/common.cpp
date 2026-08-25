#include "common.h"

unsigned long last_time = 0;
bool enable_act = true;

void setup_FFat() {
  if (!FFat.begin(true)) {
    Serial.println("[Error] FATFS Initiate Failed.");
    while (1);  // Pause
  } else {
    Serial.println("FATFS is Ready.");
  }
}
