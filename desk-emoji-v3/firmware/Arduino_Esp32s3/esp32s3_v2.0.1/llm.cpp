#include "llm.h"

LLM::LLM() {
}

String LLM::answer() {
    return llm_answer;
}

String LLM::actions() {
    return llm_actions;
}

String LLM::load_history() {
    if (!FFat.exists(HISTORY_FILE)) {
        log_info("No history file found, creating new one");
        File file = FFat.open(HISTORY_FILE, FILE_WRITE);
        if (file) {
            file.print("[]");
            file.close();
            log_info("Created new history file");
        } else {
            log_error("Failed to create new history file");
        }
        return "[]";
    }

    File file = FFat.open(HISTORY_FILE, FILE_READ);
    if (file) {
        return file.readString();
    } else {
        log_error("Failed to open history file");
    }
    return "[]";
}

void LLM::save_history(String question, String answer) {
    JsonDocument historyDoc;
    
    // First load existing history
    String existingHistory = load_history();
    if (existingHistory.length() > 0) {
        deserializeJson(historyDoc, existingHistory);
    }
    
    JsonArray historyArray = historyDoc.as<JsonArray>();
    
    // Add new question and answer
    JsonObject questionObj = historyArray.add<JsonObject>();
    questionObj["role"] = "user";
    questionObj["content"] = question;
    
    JsonObject answerObj = historyArray.add<JsonObject>();
    answerObj["role"] = "assistant";
    answerObj["content"] = answer;
    
    // Keep only MAX_MEM_LEN data.
    while (historyArray.size() > MAX_MEM_LEN * 2) {
        // Remove oldest items first (from the beginning)
        for (size_t i = 0; i < historyArray.size() - 1; i++) {
            historyArray[i] = historyArray[i + 1];
        }
        historyArray.remove(historyArray.size() - 1);
    }
    
    File file = FFat.open(HISTORY_FILE, FILE_WRITE);
    if (file) {
        if (serializeJson(historyDoc, file) > 0) {
            log_info("History saved successfully");
        } else {
            log_error("Failed to write history data");
        }
        file.close();
    } else {
        log_error("Failed to open history file");
    }
}

// memory json format:
// [
//     {
//       "role": "user",
//       "content": "question1"
//     },
//     {
//       "role": "assistant",
//       "content": "answer1"
//     },
//     {
//       "role": "user",
//       "content": "question2"
//     },
//     {
//       "role": "assistant",
//       "content": "answer2"
//     }
// ]

String LLM::chat(String question) {
    HTTPClient http;
    llm_answer = "";
    llm_actions = "";
    log_info("You: %s", question.c_str());
    
    // Create JSON document for the request
    JsonDocument doc;
    doc["model"] = MODEL;
    
    JsonArray messages = doc["messages"].to<JsonArray>();

    JsonObject systemMessage = messages.add<JsonObject>();
    systemMessage["role"] = "system";
    systemMessage["content"] = ROLE_PROMPT;

    // Parse the history JSON and add it to messages
    String history = load_history();
    if (history.length() > 0) {
        JsonDocument historyDoc;
        DeserializationError error = deserializeJson(historyDoc, history);
        
        if (!error) {
            for (JsonVariant msg : historyDoc.as<JsonArray>()) {
                JsonObject msgObj = messages.add<JsonObject>();
                msgObj["role"] = msg["role"];
                msgObj["content"] = msg["content"];
            }
        } else {
            log_error("Error parsing history JSON");
        }
    }

    JsonObject userMessage = messages.add<JsonObject>();
    userMessage["role"] = "user";
    userMessage["content"] = LLM_PROMPT + question;

    String jsonString;
    serializeJson(doc, jsonString);
    
    http.begin(API_URL);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", String("Bearer ") + API_KEY);
    
    // Add timeout settings
    http.setTimeout(10000); // 10 second timeout
    
    int httpResponseCode = http.POST(jsonString);
    
    if (httpResponseCode > 0) {
        llm_response = http.getString();
        
        // Parse the response to get the actual message
        JsonDocument responseDoc;
        DeserializationError error = deserializeJson(responseDoc, llm_response);
        
        if (!error) {
            if (responseDoc["choices"].is<JsonArray>() && responseDoc["choices"].size() > 0) {
                llm_response = responseDoc["choices"][0]["message"]["content"].as<String>();
            }
        }
    } else {
        // Improved error handling
        String errorMsg;
        switch (httpResponseCode) {
            case -1: errorMsg = "HTTPC_ERROR_CONNECTION_REFUSED"; break;
            case -2: errorMsg = "HTTPC_ERROR_SEND_HEADER_FAILED"; break;
            case -3: errorMsg = "HTTPC_ERROR_SEND_PAYLOAD_FAILED"; break;
            case -4: errorMsg = "HTTPC_ERROR_NOT_CONNECTED"; break;
            case -5: errorMsg = "HTTPC_ERROR_CONNECTION_LOST"; break;
            case -6: errorMsg = "HTTPC_ERROR_NO_STREAM"; break;
            case -7: errorMsg = "HTTPC_ERROR_NO_HTTP_SERVER"; break;
            case -8: errorMsg = "HTTPC_ERROR_TOO_LESS_RAM"; break;
            case -9: errorMsg = "HTTPC_ERROR_ENCODING"; break;
            case -10: errorMsg = "HTTPC_ERROR_STREAM_WRITE"; break;
            case -11: errorMsg = "HTTPC_ERROR_READ_TIMEOUT"; break;
            default: errorMsg = "UNKNOWN"; break;
        }
        log_error("HTTP Error: %s (%d)", errorMsg.c_str(), httpResponseCode);
        llm_response = "{\"error\": \"" + errorMsg + " (" + String(httpResponseCode) + ")\"}";
    }
    http.end();
    log_info("Bot: %s", llm_response.c_str());

    DeserializationError error = deserializeJson(doc, llm_response);
    if (!error) {
        if (doc["answer"].is<String>()) {
            llm_answer = doc["answer"].as<String>();
            save_history(question, llm_answer);
        }
        if (doc["actions"].is<JsonArray>()) {
            JsonArray actionArray = doc["actions"].as<JsonArray>();
            String actionList = "";
            for (JsonVariant action : actionArray) {
                actionList += action.as<String>() + ",";
            }
            if (actionList.length() > 0) {
                actionList.remove(actionList.length() - 1); // Remove trailing comma
            }
            llm_actions = actionList;
        }
    }

    return llm_response;
}

void LLM::stream_chat(String question) {
    // doc["stream"] = true;
//   // 准备请求数据
//   String messages = "[{\"role\":\"system\",\"content\":\"你是一个有帮助的助手。\"},{\"role\":\"user\",\"content\":\"" + userMessage + "\"}]";
//   String requestBody = "{\"model\":\"gpt-4\",\"messages\":" + messages + ",\"stream\":true}";
  
//   // 发送HTTP请求
//   client.println("POST /v1/chat/completions HTTP/1.1");
//   client.println("Host: api.openai.com");
//   client.println("Content-Type: application/json");
//   client.print("Authorization: Bearer ");
//   client.println(apiKey);
//   client.print("Content-Length: ");
//   client.println(requestBody.length());
//   client.println("Connection: close");
//   client.println();
//   client.println(requestBody);

//   // 等待服务器响应
//   while (client.connected()) {
//     String line = client.readStringUntil('');
//     if (line == "\r") {
//       Serial.println("响应头接收完毕");
//       break;
//     }
//   }

//   // 处理流式响应
//   String fullResponse = "";
//   String buffer = "";
//   bool inData = false;
  
//   while (client.connected() || client.available()) {
//     if (client.available()) {
//       char c = client.read();
//       buffer += c;
      
//       // 当遇到data:前缀时开始处理
//       if (buffer.endsWith("data: ")) {
//         inData = true;
//         buffer = "";
//       } 
//       // 当在data内部且遇到换行符时，处理数据
//       else if (inData && c == '') {
//         inData = false;
        
//         // 跳过"[DONE]"消息
//         if (buffer.indexOf("[DONE]") == -1) {
//           DynamicJsonDocument doc(1024);
//           DeserializationError error = deserializeJson(doc, buffer);
          
//           if (!error) {
//             if (doc.containsKey("choices") && doc["choices"][0].containsKey("delta") && 
//                 doc["choices"][0]["delta"].containsKey("content")) {
//               String content = doc["choices"][0]["delta"]["content"].as<String>();
//               fullResponse += content;
//               Serial.print(content); // 实时打印内容
//             }
//           }
//         }
//         buffer = "";
//       }
//     }
//   }
}