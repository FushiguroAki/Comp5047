#include <Arduino.h>
#include <WiFi.h>
#include "Audio.h"
#include "CloudSpeechClient.h"
#include "network_param.h"
#include "SPIFFS.h"

#include "../lib/main.h"
#include "../lib/PublicResource.h"


//开启特殊模块(自带线程)
ColorSensorModule CSM;

bool voiceActivated = false;
// Initialize the audio and speech recognition client
Audio* audio;
CloudSpeechClient* speechClient;


void setup()
{
    Serial.begin(9600);
    delay(500);

    // connect Wifi
    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");

    Serial.println("System initialized. Waiting for voice command...");

    // Initialize audio and speech client
    audio = new Audio(); 
    speechClient = new CloudSpeechClient(USE_APIKEY);
}


void loop() {
    if (!voiceActivated) {
        Serial.println("Recording audio for voice command...");
        audio->Record();

        audio->SaveToFile("/recorded_audio.wav");
        
        Serial.println("Transcribing voice command...");
        speechClient->Transcribe(audio);

        // Check if the recognized voice command activates the system
        if (speechClient->isCommandRecognized("activate")) {
            voiceActivated = true;
            Serial.println("Voice command recognized. Activating system...");
        } else {
            Serial.println("Voice command not recognized.");
        }
    }

    if (voiceActivated) {
        Serial.println("System activated.");
        // Run modules only when the system is activated by voice
        // CM.ColorModuleLoop();
        // ILM.IndicatorLightLoop();
        // 如果需要，添加其他模块的循环调用
        // AM.AmmoModuleLoop();
        // TM.TriggerModuleLoop();
        // Add other module loops as needed
    }

    delay(500);
}