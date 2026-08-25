#include "net.h"
#include "oled.h"
#include "sensor.h"
#include "audio.h"
#include "emoji.h"
#include "head.h"
#include "cmd.h"
#include "act.h"
#include "animation.h"

unsigned long loop_start_time;

WifiClient wifiClient;
AsrClient asrClient;
TtsClient ttsClient;
LLM llm;

void websocket_loop() {
    asrClient.loop();
    ttsClient.loop();
}

void setup() {
  Serial.begin(115200);
  Serial.flush();
  // Set default log level
  log_set_level(LOG_LEVEL_INFO);
  log_info("Initializing...");
  setup_oled();
  setup_FFat();
  setup_head();
  setup_led();
  wifiClient.setup_wifi();
  wifiClient.setup_udp();
  setup_sensor();
  setup_audio();
  oled_println("\nConnect Server...");
  asrClient.connect();
  // ttsClient.connect();
  emoji_init();
  log_info("Desk-Emoji is Ready.");
  last_time = millis();
}

void loop() {
  handle_gesture();
  handle_cmd();
  asrClient.connect();
  // ttsClient.connect();
  websocket_loop();

  if (millis() - loop_start_time > 1000) {
    wifiClient.send_ip();
    loop_start_time = millis();
  }

  if (start_chat) {
    start_chat = false;
    loop_start_time = millis();
    while (millis() - loop_start_time < 600000) {  // max chat time 10 minutes
      asrClient.connect();
      websocket_loop();

      // Speech-to-Text
      eye_happy();
      if (!asrClient.ASR()) {
        log_error("ASR Failed!");
      }
      websocket_loop();
      asrClient.disconnect();
      if (asrClient.asrResult() == "") {
        log_info("Exit Chat...");
        start_chat = false;
        blink_led(COLOR_RED, 3);
        break;
      }

      // Thinking...
      ttsClient.connect();
      set_led(COLOR_YELLOW, 10);
      llm.chat(asrClient.asrResult());
      websocket_loop();
      async_sequent_act(llm.actions());

      // Text-to-Speech
      if (!ttsClient.TTS(llm.answer())) {
        log_error("TTS Failed!");
      }
      websocket_loop();
    }
  }

  if (enable_act && millis() - last_time > random(6, 10) * 1000) {
    random_act();
    last_time = millis();
  }

  delay(10);
}

// {"factory": "reboot"}
// {"factory": "reset_wifi"}
// {"factory": "adjust_x -10"}   -left, +right
// {"factory": "adjust_y -10"}    -up, +down
