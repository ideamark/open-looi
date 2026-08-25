#ifndef NET_H
#define NET_H

#include <WiFi.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include "common.h"
#include "cmd.h"

#define WIFI_CONF_FILE "/WIFI.conf"
#define AP_SSID "Desk-Emoji"
#define UDP_PORT 4210

class WifiClient {

public:
  WifiClient();
  void setup_wifi();
  void setup_udp();
  void send_broadcast_msg(String msg);
  void send_ip();

private:
  WiFiUDP udp;
  WebServer server;
  IPAddress broadcastIP;
  bool done_config = false;
  String ssid = "";
  String password = "";
  String local_ip = "";
  String local_mac = "";
  const int MAX_RECONNECT_ATTEMPTS = 20;
  static const char index_html[] PROGMEM;

  void config_wifi();
  void setup_http_server();
};

void reset_wifi();

#endif
