#include <stdio.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>

#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 250
#define MAX_WIFI_INIT_RETRY 50
#define BUZZER_PIN 2


int Max;
//char character1, character2 ;
String content = "";
char data_buffer[512*3];
int data_count = 0;

const char* wifi_ssid="Thanunya's iPhone";
const char* wifi_passwd="beambeam";

HTTPClient http;
String HOST_NAME = "http://protected-wave-45390.herokuapp.com/";

void connectWiFi(){
  Serial.println("Connecting to WiFi AP...........");
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid,wifi_passwd);
  while((WiFi.status()!=WL_CONNECTED)){
    delay(WIFI_RETRY_DELAY);
    Serial.print("#");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address:");
  Serial.println(WiFi.localIP());
  
}

void setup() {
  Serial.begin(115200) ;
  Serial.println(".... initializing ....") ;
  pinMode(BUZZER_PIN , OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT) ;
  Max = 110;
  connectWiFi();
}

void workWithServer(int c){
  StaticJsonBuffer<50> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
  JSONencoder["bpm"] = c;

  char JSONmessageBuffer[50];
  JSONencoder.prettyPrintTo(JSONmessageBuffer , sizeof(JSONmessageBuffer));

  http.begin(HOST_NAME + "api/sensor/");

  http.addHeader("Content-Type" , "application/json");

  int httpCode = http.POST(JSONmessageBuffer);
  
  Serial.println(httpCode);

  if(httpCode == 200){
    const size_t bufferSize = JSON_OBJECT_SIZE(3);
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& resp = jsonBuffer.parseObject(http.getString());
    if (resp["max_bpm"]!=0) Max = resp["max_bpm"];
    Serial.println(Max);
    if(c >= Max){
      digitalWrite(BUZZER_PIN, HIGH);
    }
    else digitalWrite(BUZZER_PIN, LOW);
  
  }
  http.end();
}

void loop() {

  if(Serial.available()) {
//     character1 =  Serial.read();
//     character2 = Serial.read();
//     Serial.printf("%d\n", 256*(int)character1 + (int)character2);
    content = Serial.readString();
//  Serial.printf("%s\n", content.c_str());
    int test = 0, i;
    for(i = 0; content[i] != '\0' && i < 10; ++i) {
      test = 10*test + (content[i] - '0');
    }
    Serial.printf(">> %d\n", test);
    workWithServer(test);
  }
  delay(10000);
  
}
