#include "common.h"

unsigned long last_time = 0;
bool enable_act = true;
bool start_chat = false;

void setup_FFat() {
  if (!FFat.begin(true)) {
    log_error("FATFS Initiate Failed.");
    while (true);  // Pause
  } else {
    log_info("FATFS is Ready.");
  }
}

String get_mac_address() {
  uint8_t mac_address[6];
  WiFi.macAddress(mac_address);
  char mac_str[18];
  sprintf(mac_str, "%02X:%02X:%02X:%02X:%02X:%02X", 
          mac_address[0], mac_address[1], mac_address[2], 
          mac_address[3], mac_address[4], mac_address[5]);
  return String(mac_str);
}

String generate_uuid() {
    time_t now = time(nullptr);
    char uuid[37];
    
    // Get MAC address directly from WiFi
    uint8_t mac_bytes[6];
    WiFi.macAddress(mac_bytes);
    
    // Format UUID using MAC address, current time, and random values
    sprintf(uuid, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        mac_bytes[0], mac_bytes[1], mac_bytes[2], (uint8_t)(now >> 24),
        (uint8_t)(now >> 16), (uint8_t)(now >> 8),
        (uint8_t)now, mac_bytes[3],
        mac_bytes[4], mac_bytes[5], 
        random(0xFF), random(0xFF), random(0xFF), random(0xFF), random(0xFF), random(0xFF));
    
    return String(uuid);
}

String get_byte_str(uint8_t* payload, size_t length) {
    String byteStr = "";
    for(int i = 0; i < length; i++) {
        byteStr += "\\x";
        char hexStr[3];
        sprintf(hexStr, "%02x", payload[i]);
        byteStr += hexStr;
    }
    return byteStr;
}

String get_local_ip() {
  return WiFi.localIP().toString();
}
