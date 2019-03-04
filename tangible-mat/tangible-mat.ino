#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

int irAnalogPin = A0;
int restAreaButtonPin = 0;
int coffeeButtonPin = 1;
int musicButtonPin = 2;
int lightButtonPin = 3;
int nightLightButtonPin = 4;

int nightLightOutputPin = 5;
int lightstripClockPin = 6;
int lightstripOutputPin = 7;

struct RawInput{
  boolean musicButton;
  boolean coffeeButton;
};

struct ProcessedInput {
  boolean musicButtonPressed;
  boolean musicButtonHold;
  boolean coffeeButtonPressed;
};

RawInput lastCycleRawInput = {
  musicButton: false,
  coffeeButton: false
};

RawInput currentRawInput = {
  musicButton: false,
  coffeeButton: false
};

ProcessedInput processedInput = {
  musicButtonPressed: false,
  musicButtonHold: false,
  coffeeButtonPressed: false
};

unsigned long musicButtonStartHold = 0;

int tpNotConnected = 0;
int tpConnecting = 1;
int tpConnected = 2;
int tpFailedToConnect = 3;
int tpLinkCloudConnection = notConnected;
String tpLinkToken = "";

void setup() {
  pinMode(coffeeButtonPin, INPUT);
  pinMode(musicButtonPin, INPUT);

  Serial.begin(9600);
  connectToWifi();
}

void loop() {
  if(WiFi.status() == WL_CONNECTED && tpLinkCloudConnection == tpNotConnected){
    loginToIotLampCloud();
  }
  readMatInput();
  processMatInput();
  if(processedInput.musicButtonPressed) {
    toggleMusic();
  }
  if(processedInput.musicButtonHold) {
    changeVolume();
  }
  if(processedInput.coffeeButtonPressed) {
    toggleCoffee();
  }
}

void readMatInput() {
  currentRawInput = {
    musicButton: digitalRead(musicButtonPin),
    coffeeButton: digitalRead(coffeeButtonPin)
  };
}

void processMatInput() {
  processedInput = {
    musicButtonPressed: false,
    musicButtonHold: false,
    coffeeButtonPressed: false
  };
  
  //Music button
  if(lastCycleRawInput.musicButton == HIGH && currentRawInput.musicButton == LOW) {
    processedInput.musicButtonPressed = true;
  } else if(lastCycleRawInput.musicButton == HIGH && currentRawInput.musicButton == HIGH &&
        millis() - musicButtonStartHold > 1000) {
    processedInput.musicButtonHold = true;
  } else if(lastCycleRawInput.musicButton == LOW && currentRawInput.musicButton == HIGH) {
    musicButtonStartHold = millis();
  }


  //Coffee button
  if(lastCycleRawInput.musicButton == HIGH && currentRawInput.musicButton == LOW) {
    processedInput.coffeeButtonPressed = true;
  }

  lastCycleRawInput = currentRawInput;
}

void connectToWifi() {
  WiFi.begin("tangi19", "Tangible19iot");   //WiFi connection
}

void loginToIotLampCloud() {
  Serial.println("Logging in to tp link cloud");
  tpLinkCloudConnection = tpConnecting;
  StaticJsonBuffer<200> jsonBuffer;
  
  HTTPClient http;
  http.begin("https://wap.tplinkcloud.com");
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST("{\"method\": \"login\",\"params\": {\"appType\": \"Kasa_Android\",\"cloudUserName\": \"stina.vk@gmail.com\",\"cloudPassword\": \"Tangible\",\"terminalUUID\": \"015ea6b4-26f4-4d50-af6f-d543024f57b8\"}}");
  Serial.println("Login post request http code: " + httpCode);
  String payload = http.getString();
  http.end();
  JsonObject& root = jsonBuffer.parseObject(payload);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    tpLinkCloudConnection = tpFailedToConnect;
    return;
  } else {
    tpLinkToken = root["token"];
    tpLinkCloudConnection = tpConnected;
    Serial.println("Got token and connected to the tp link cloud");
  }
}
