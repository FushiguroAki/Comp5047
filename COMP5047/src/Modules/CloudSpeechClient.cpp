#include "CloudSpeechClient.h"
#include "network_param.h"
#include <base64.h>
#include <ArduinoJson.h>

CloudSpeechClient::CloudSpeechClient(Authentication authentication) {
  this->authentication = authentication;
  this->commandRecognized = false;

  client.setCACert(root_ca);
  if (!client.connect(server, 443)) {
    Serial.println("Connection to server failed!");
  }
}

CloudSpeechClient::~CloudSpeechClient() {
  client.stop();
}

void CloudSpeechClient::PrintHttpBody(Audio* audio) {
  // 编码WAV文件头并发送
  String encodedHeader = base64::encode(audio->paddedHeader, sizeof(audio->paddedHeader));
  encodedHeader.replace("\n", ""); // 删除末尾的换行符
  Serial.println("Encoded WAV Header:");
  Serial.println(encodedHeader);
  client.print(encodedHeader);     // 发送HTTP请求主体部分

  // 编码音频数据并发送
  char** wavData = audio->wavData;
  for (int j = 0; j < audio->wavDataSize / audio->dividedWavDataSize; ++j) {
    String encodedData = base64::encode((byte*)wavData[j], audio->dividedWavDataSize);
    encodedData.replace("\n", ""); // 删除末尾的换行符
    //Serial.print("Encoded Audio Data Part "); // 调试信息
    //Serial.print(j);
    //Serial.println(":");
    //Serial.println(encodedData); // 打印编码的音频数据
    client.print(encodedData);     // 发送HTTP请求主体部分
  }
}

void CloudSpeechClient::Transcribe(Audio* audio) {
  // 准备HTTP请求头和主体
  String HttpBody1 = "{\"config\":{\"encoding\":\"LINEAR16\",\"sampleRateHertz\":16000,\"languageCode\":\"en-US\"},\"audio\":{\"content\":\"";
  String HttpBody3 = "\"}}\r\n\r\n";
  int httpBody2Length = (audio->wavDataSize + sizeof(audio->paddedHeader)) * 4 / 3; // Base64编码
  String ContentLength = String(HttpBody1.length() + httpBody2Length + HttpBody3.length());

  String HttpHeader;
  HttpHeader = String("POST /v1/speech:recognize?key=") + ApiKey
               + String(" HTTP/1.1\r\nHost: speech.googleapis.com\r\nContent-Type: application/json\r\nContent-Length: ") + ContentLength + String("\r\n\r\n");

  // 发送HTTP请求
  client.print(HttpHeader);
  client.print(HttpBody1);
  PrintHttpBody(audio);
  client.print(HttpBody3);

  // 接收和处理服务器响应
  String response = "";
  while (!client.available());
  while (client.available()) {
    char c = client.read();
    response += c;
  }

  Serial.println("Response: " + response);

  // 解析JSON响应
  int jsonStart = response.indexOf('{');
  if (jsonStart >= 0) {
    String jsonResponse = response.substring(jsonStart);
    StaticJsonDocument<1024> jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, jsonResponse);

    if (!error) {
      const char* transcript = jsonDoc["results"][0]["alternatives"][0]["transcript"];
      Serial.print("Recognized command: ");
      Serial.println(transcript);

      // 检查识别的文本是否包含特定的命令
      if (String(transcript).indexOf("activate") >= 0) {
        commandRecognized = true;
      } else {
        commandRecognized = false;
      }
    } else {
      Serial.println("Failed to parse JSON");
      commandRecognized = false;
    }
  }
}

bool CloudSpeechClient::isCommandRecognized(const String& command) {
  return commandRecognized;
}