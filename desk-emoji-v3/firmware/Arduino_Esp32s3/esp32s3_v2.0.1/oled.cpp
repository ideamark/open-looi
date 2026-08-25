#include "oled.h"

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup_oled() {
  if (!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    log_error("OLED Initiate Failed.");
    while (true);  // Pause
  } else {
    log_info("OLED is Ready.");
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    oled.setCursor(0, 0);
    oled_println("Desk-Emoji v" + String(VERSION), 500);
  }
}

void oled_print(String text, int delay_time) {
  oled.print(text);
  oled.display();
  delay(delay_time);
}

void oled_println(String text, int delay_time) {
  oled.println(text);
  oled.display();
  delay(delay_time);
}
