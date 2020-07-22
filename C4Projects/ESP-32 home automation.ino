/*
 C4Projects
 Capstone project "IOT based HomeAutomation using Alexa"
 Name= Nirmal kumar
 Github::https://github.com/NirmalKnock
*/ 
int white_light=16;
int light=4;
int light1=18;
//required header files
#include <String.h>
#
#include <Arduino.h>


#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebSocketsClient.h> 
#include <ArduinoJson.h> 
#include <StreamString.h>

WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;


#define MyApiKey "ff817bda-9f72-4e56-85f7-dee9961159a3" //  sinric API Key. 
#define MySSID "nirmal kumar" //  Wifi network SSID
#define MyWifiPassword "nirmal@1234" //  Wifi network password

#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;




void turnOn(String deviceId) {
  if (deviceId == "5d850b87aa5bc849af8b16fe") // Device ID of first device
  {  
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(white_light,HIGH); //lights ON
  } 
  else if (deviceId == "5d86173c554c6f0923042d57") // Device ID of second device
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(light,HIGH);
  }
  else {
    Serial.print("5d8e63a1acb3630272a5082f");
    Serial.println(deviceId);
    digitalWrite(light1,HIGH);    
  }     
}

void turnOff(String deviceId) {
   if (deviceId == "5d850b87aa5bc849af8b16fe") // Device ID of first device
   {  
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     digitalWrite(white_light,LOW);
   }
   else if (deviceId == "5d86173c554c6f0923042d57") // Device ID of second device
   { 
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     digitalWrite(light,LOW);
  }
  else {
     Serial.print("Turn off for unknown device id: ");
     Serial.println(deviceId); 
     digitalWrite(light1,LOW);   
  }
}



void setup() {
  Serial.begin(115200);
  pinMode(white_light,OUTPUT);
  pinMode(light,OUTPUT);
  pinMode(light1,OUTPUT);
  
  WiFiMulti.addAP(MySSID, MyWifiPassword);
  Serial.println();
  Serial.print("Connecting to Wifi: ");
  Serial.println(MySSID);  

  // Waiting for Wifi connect
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if(WiFiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("WiFi connected. ");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // server address, port and URL
  webSocket.begin("iot.sinric.com", 80, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", MyApiKey);
  
  // try again every 5000ms if connection has failed
  webSocket.setReconnectInterval(5000);  
}

void loop() {
  webSocket.loop();
  float h = dht.readHumidity();
  float t = dht.readTemperature(); 
  delay(100); 
  
  if(isConnected) {
      uint64_t now = millis();
      
      // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night
      if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
          heartbeatTimestamp = now;
          webSocket.sendTXT("H");          
      }
  }   
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      isConnected = false;    
      Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
      break;
    case WStype_CONNECTED: {
      isConnected = true;
      Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
      Serial.printf("Waiting for commands from sinric.com ...\n");        
      }
      break;
    case WStype_TEXT: {
        Serial.printf("[WSc] get text: %s\n", payload);
       
#if ARDUINOJSON_VERSION_MAJOR == 5
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6        
        DynamicJsonDocument json(1024);
        deserializeJson(json, (char*) payload);      
#endif        
        String deviceId = json ["deviceId"];     
        String action = json ["action"];
        
        if(action == "setPowerState") {      // Switch or Light
            String value = json ["value"];
            if(value == "ON") {
                turnOn(deviceId);
            } else {
                turnOff(deviceId);
            }
        }
        else if (action == "SetTargetTemperature") {
            String deviceId = json ["deviceId"];     
            String action = json ["action"];
            String value = json ["value"];
        }
        else if (action == "test") {
            Serial.println("[WSc] received test command from sinric.com");
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }
}
 
