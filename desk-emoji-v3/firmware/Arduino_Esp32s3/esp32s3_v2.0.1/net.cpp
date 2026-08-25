#include "net.h"

const char WifiClient::index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>WiFi 配置</title>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 20px;
      background-color: #f5f5f5;
      color: #333;
    }
    .container {
      max-width: 800px;
      margin: 0 auto;
      padding: 20px;
      background-color: white;
      border-radius: 8px;
      box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
    }
    h1 {
      color: #0066cc;
      text-align: center;
      margin-bottom: 30px;
    }
    h2 {
      color: #009688;
      border-bottom: 1px solid #eee;
      padding-bottom: 10px;
    }
    .info-section {
      margin-bottom: 30px;
    }
    .status {
      background-color: #e8f5e9;
      padding: 15px;
      border-radius: 5px;
      margin: 20px 0;
    }
    .footer {
      text-align: center;
      margin-top: 30px;
      font-size: 0.9em;
      color: #666;
    }
    button {
      background-color: #4CAF50;
      border: none;
      color: white;
      padding: 10px 20px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 16px;
      margin: 10px 2px;
      cursor: pointer;
      border-radius: 4px;
    }
    #networks-list {
      list-style-type: none;
      padding: 0;
    }
    .network-item {
      padding: 12px 15px;
      border-bottom: 1px solid #ddd;
      cursor: pointer;
      transition: background-color 0.3s;
      display: flex;
      justify-content: space-between;
      align-items: center;
    }
    .network-item:hover {
      background-color: #f0f0f0;
    }
    .network-item.selected {
      background-color: #e3f2fd;
    }
    .wifi-strength {
      margin-left: 10px;
      font-size: 0.9em;
      color: #666;
    }
    .password-form {
      margin-top: 20px;
      padding: 15px;
      background-color: #f9f9f9;
      border-radius: 5px;
      display: none;
    }
    input[type="text"], input[type="password"] {
      width: 100%;
      padding: 8px;
      margin: 8px 0;
      box-sizing: border-box;
      border: 1px solid #ddd;
      border-radius: 4px;
    }
    .message {
      padding: 10px;
      margin: 10px 0;
      border-radius: 4px;
    }
    .success {
      background-color: #d4edda;
      color: #155724;
    }
    .error {
      background-color: #f8d7da;
      color: #721c24;
    }
    .spinner {
      border: 4px solid rgba(0, 0, 0, 0.1);
      width: 20px;
      height: 20px;
      border-radius: 50%;
      border-top: 4px solid #007bff;
      animation: spin 1s linear infinite;
      display: inline-block;
      margin-right: 10px;
      vertical-align: middle;
    }
    @keyframes spin {
      0% { transform: rotate(0deg); }
      100% { transform: rotate(360deg); }
    }
    .hidden {
      display: none;
    }
    .scan-btn {
      background-color: #007bff;
      margin-bottom: 20px;
    }
    .loading-text {
      margin-left: 10px;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>WiFi 配置</h1>
    
    <div class="status">
      <p><strong>状态:</strong> Desk-Emoji 准备配置 Wi-Fi 网络</p>
    </div>
    
    <div class="info-section">
      <h2>可用 Wi-Fi 网络</h2>
      <p>请选择一个网络连接：</p>
      
      <button id="scan-btn" class="scan-btn" onclick="scanNetworks()">
        <span id="scan-spinner" class="spinner hidden"></span>
        <span id="scan-text">扫描网络</span>
      </button>
      
      <div id="message" class="message hidden"></div>
      
      <ul id="networks-list"></ul>
      
      <div id="password-form" class="password-form">
        <h3 id="selected-network">网络名称</h3>
        <form id="wifi-form">
          <input type="hidden" id="ssid-input" name="ssid">
          <label for="password-input">密码：</label>
          <input type="password" id="password-input" name="password" placeholder="请输入密码">
          <button type="submit">保存配置</button>
        </form>
      </div>
    </div>
    
    <div class="footer">
      <p>Desk-Emoji 杭州易问科技 | &copy; 2024</p>
    </div>
  </div>

  <script>
    let selectedNetwork = null;
    
    // Scan for networks
    function scanNetworks() {
      const scanBtn = document.getElementById('scan-btn');
      const scanSpinner = document.getElementById('scan-spinner');
      const scanText = document.getElementById('scan-text');
      const messageDiv = document.getElementById('message');
      const networksList = document.getElementById('networks-list');
      
      // Show loading state
      scanSpinner.classList.remove('hidden');
      scanText.innerText = '扫描中...';
      scanBtn.disabled = true;
      messageDiv.classList.add('hidden');
      networksList.innerHTML = '';
      
      // Hide password form
      document.getElementById('password-form').style.display = 'none';
      
      // Make request to scan endpoint
      fetch('/scan-wifi')
        .then(response => response.json())
        .then(data => {
          // Reset scan button
          scanSpinner.classList.add('hidden');
          scanText.innerText = '扫描网络';
          scanBtn.disabled = false;
          
          if (data.length === 0) {
            messageDiv.innerHTML = '未找到网络';
            messageDiv.className = 'message error';
            messageDiv.classList.remove('hidden');
            return;
          }
          
          // Display networks
          data.forEach(network => {
            const listItem = document.createElement('li');
            listItem.className = 'network-item';
            listItem.setAttribute('data-ssid', network.ssid);
            
            // Determine signal strength icon
            let strengthText = '';
            if (network.rssi > -50) {
              strengthText = '强';
            } else if (network.rssi > -70) {
              strengthText = '优';
            } else if (network.rssi > -80) {
              strengthText = '中';
            } else {
              strengthText = '弱';
            }
            
            listItem.innerHTML = `
              <span>${network.ssid}</span>
              <span class="wifi-strength">${strengthText} (${network.rssi} dBm)</span>
            `;
            
            listItem.addEventListener('click', () => selectNetwork(network.ssid));
            networksList.appendChild(listItem);
          });
        })
        .catch(error => {
          scanSpinner.classList.add('hidden');
          scanText.innerText = '扫描网络';
          scanBtn.disabled = false;
          
          messageDiv.innerHTML = '扫描网络错误: ' + error.message;
          messageDiv.className = 'message error';
          messageDiv.classList.remove('hidden');
        });
    }
    
    // Select a network
    function selectNetwork(ssid) {
      selectedNetwork = ssid;
      
      // Update UI to show the selected network
      const networkItems = document.querySelectorAll('.network-item');
      networkItems.forEach(item => {
        if (item.getAttribute('data-ssid') === ssid) {
          item.classList.add('selected');
        } else {
          item.classList.remove('selected');
        }
      });
      
      // Show password form
      const passwordForm = document.getElementById('password-form');
      document.getElementById('selected-network').innerText = ssid;
      document.getElementById('ssid-input').value = ssid;
      passwordForm.style.display = 'block';
      
      // Focus password field
      document.getElementById('password-input').focus();
    }
    
    // Submit form handler
    document.getElementById('wifi-form').addEventListener('submit', function(e) {
      e.preventDefault();
      
      const ssid = document.getElementById('ssid-input').value;
      const password = document.getElementById('password-input').value;
      const messageDiv = document.getElementById('message');
      const networksList = document.getElementById('networks-list');
      
      if (!ssid) {
        messageDiv.innerHTML = '请选择一个网络';
        messageDiv.className = 'message error';
        messageDiv.classList.remove('hidden');
        return;
      }
      
      // Show saving message
      messageDiv.innerHTML = '保存配置中...';
      messageDiv.className = 'message';
      messageDiv.classList.remove('hidden');
      
      // Send data to server
      fetch('/save-wifi', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/x-www-form-urlencoded',
        },
        body: `ssid=${encodeURIComponent(ssid)}&password=${encodeURIComponent(password)}`
      })
      .then(response => response.json())
      .then(data => {
        if (data.success) {
          messageDiv.innerHTML = 'WiFi 设置成功！';
          messageDiv.className = 'message success';
          networksList.innerHTML = '';
        } else {
          messageDiv.innerHTML = '错误: ' + data.message;
          messageDiv.className = 'message error';
        }
      })
      .catch(error => {
        messageDiv.innerHTML = '保存配置错误: ' + error.message;
        messageDiv.className = 'message error';
      });
    });
    
    // Start by scanning networks on page load
    window.onload = function() {
      // Wait a second before scanning to ensure the page is fully loaded
      setTimeout(scanNetworks, 1000);
    };
  </script>
</body>
</html>
)rawliteral";

WifiClient::WifiClient() : server(80) {
  broadcastIP = IPAddress(255, 255, 255, 255);
}

void WifiClient::setup_wifi() {
  int connection_attempts = 0;

  oled_print("\nConnect WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    if (FFat.exists(WIFI_CONF_FILE)) {
      File wifiFile = FFat.open(WIFI_CONF_FILE, FILE_READ);
      if (wifiFile) {
        ssid = wifiFile.readStringUntil('\n');
        password = wifiFile.readStringUntil('\n');
        wifiFile.close();
        ssid.trim();
        password.trim();
        log_info("WiFi credentials loaded from file");
      } else {
        log_error("Failed to open WiFi config file");
        delay(1000);
        continue;
      }
    } else {
      config_wifi();
    }

    if (ssid.length() > 0) {
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid.c_str(), password.c_str());
      connection_attempts++;
      log_info("Connecting to WiFi...");
      blink_led();
      oled_print(".", 1000);
      if (connection_attempts > MAX_RECONNECT_ATTEMPTS) {
        reset_wifi();
        config_wifi();
      }
    } else {
      log_error("WiFi config file is empty");
      delay(500);
      continue;
    }
  }
  log_info("WiFi connected");
  local_ip = get_local_ip();
  oled_println("\n\nIP: " + local_ip);
  log_info("IP address: %s", local_ip.c_str());
  local_mac = get_mac_address();
  log_info("MAC address: %s", local_mac.c_str());
}

void WifiClient::config_wifi() {
  log_error("Failed to connect to WiFi, starting WiFi config...");
  WiFi.disconnect();
  set_led(COLOR_RED, 20);
  setup_http_server();
  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled_println("Desk-Emoji v" + String(VERSION), 500);
  oled_print("\nConnect WiFi");
}

void WifiClient::setup_http_server() {
  done_config = false;  

  // Set up Access Point (open network - no password)
  log_info("Opening AP...");
  WiFi.softAP(AP_SSID);

  // Route for root / web page
  server.on("/", HTTP_GET, [this]() {
    server.send(200, "text/html", index_html);
  });
  
  // Route for scanning WiFi networks
  server.on("/scan-wifi", HTTP_GET, [this]() {
    String json = "[";
    int n = WiFi.scanNetworks();
    
    for (int i = 0; i < n; ++i) {
      if (i > 0) json += ",";
      json += "{";
      json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
      json += "\"rssi\":" + String(WiFi.RSSI(i));
      json += "}";
    }
    json += "]";
    
    server.send(200, "application/json", json);
    WiFi.scanDelete();
  });
  
  // Route for saving WiFi credentials
  server.on("/save-wifi", HTTP_POST, [this]() {
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    
    if (ssid.length() == 0) {
      server.send(400, "application/json", "{\"success\":false,\"message\":\"SSID cannot be empty\"}");
      return;
    }
    
    // Create/open the wifi.conf file for writing
    File configFile = FFat.open(WIFI_CONF_FILE, FILE_WRITE);
    if (!configFile) {
      server.send(500, "application/json", "{\"success\":false,\"message\":\"Failed to open config file for writing\"}");
      return;
    }
    
    // Write the WiFi credentials to the file
    configFile.println(ssid);
    configFile.println(password);
    configFile.close();
    
    log_info("WiFi credentials saved:");
    log_info("ssid: %s", ssid.c_str());
    log_info("password: %s", password.c_str());
    
    server.send(200, "application/json", "{\"success\":true,\"message\":\"WiFi configuration saved\"}");
    done_config = true;
  });
  
  // Start server
  server.begin();
  
  // Get the correct IP address from softAP
  IPAddress ip = WiFi.softAPIP();
  log_info("HTTP server started at %s", ip.toString().c_str());
  
  // Display IP address on OLED screen
  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled_println("WiFi Config Mode");
  oled_println("");
  oled_print("SSID: ");
  oled_println(AP_SSID);
  oled_println("");
  oled_print("IP: ");
  oled_println(ip.toString());
  
  while (!done_config) {
    server.handleClient();
    handle_cmd();
    delay(10);
  }
  // Release server resources
  server.close();
  WiFi.disconnect();
  blink_led(COLOR_GREEN, 3, 100);
  log_info("HTTP server stopped");
}

void WifiClient::setup_udp() {
  udp.begin(UDP_PORT);
  log_info("UDP server started");
}

void WifiClient::send_broadcast_msg(String msg) {
  udp.beginPacket(broadcastIP, UDP_PORT);
  udp.print(msg);
  udp.endPacket();
  log_debug("Send Broadcast: %s", msg.c_str());
}

void WifiClient::send_ip() {
  send_broadcast_msg("Desk-Emoji_" + local_ip);
}

void reset_wifi() {
  if (FFat.exists(WIFI_CONF_FILE)) {
    FFat.remove(WIFI_CONF_FILE);
    log_info("WiFi Config File Deleted");
    delay(500);
    ESP.restart();
  }
}
