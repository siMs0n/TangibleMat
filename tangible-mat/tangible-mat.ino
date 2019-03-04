#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

int irAnalogPin = A0;
int restAreaButtonPin = 0;
int coffeeButtonPin = 1;
int musicButtonPin = 2;
int iotLampButtonPin = 3;
int nightLightButtonPin = 4;

int nightLightOutputPin = 5;
int lightstripClockPin = 6;
int lightstripOutputPin = 7;

struct RawInput{
  boolean musicButton;
  boolean coffeeButton;
  boolean iotLampButton;
};

struct ProcessedInput {
  boolean musicButtonPressed;
  boolean musicButtonHold;
  boolean coffeeButtonPressed;
  boolean iotLampButtonPressed;
};

struct MatState {
  int iotLampState;
  int iotLampBrightness;
};

RawInput lastCycleRawInput = {
  musicButton: false,
  coffeeButton: false,
  iotLampButton: false
};

RawInput currentRawInput = {
  musicButton: false,
  coffeeButton: false,
  iotLampButton: false
};

ProcessedInput processedInput = {
  musicButtonPressed: false,
  musicButtonHold: false,
  coffeeButtonPressed: false,
  iotLampButtonPressed: false
};

MatState matState = {
  iotLampState: 0,
  iotLampBrightness: 50
};

unsigned long musicButtonStartHold = 0;

int tpNotConnected = 0;
int tpConnecting = 1;
int tpConnected = 2;
int tpFailedToConnect = 3;
int tpLinkCloudConnection = tpNotConnected;
String tpLinkToken = "";
String tpLinkLampDeviceId = "";

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

  if(processedInput.iotLampButtonPressed) {
    toggleIotLamp();
  }
}

void readMatInput() {
  currentRawInput = {
    musicButton: digitalRead(musicButtonPin),
    coffeeButton: digitalRead(coffeeButtonPin),
    iotLampButton: digitalRead(iotLampButtonPin)
  };
}

void processMatInput() {
  processedInput = {
    musicButtonPressed: false,
    musicButtonHold: false,
    coffeeButtonPressed: false,
    iotLampButtonPressed: false
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

  //Iot lamp button
  if(lastCycleRawInput.iotLampButton == HIGH && currentRawInput.iotLampButton == LOW) {
    processedInput.iotLampButtonPressed = true;
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
    const char* charToken = root["token"];
    tpLinkToken = String(charToken);
    fetchTpLinkDeviceId();
    Serial.println("Got tp link token");
  }
}

void fetchTpLinkDeviceId() {
  Serial.println("Getting the device id from the tp link cloud");
  StaticJsonBuffer<200> jsonBuffer;
  
  HTTPClient http;
  http.begin("https://wap.tplinkcloud.com?token=" + tpLinkToken);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST("{\"method\": \"getDeviceList\"}");
  Serial.println("Get device list post request http code: " + httpCode);
  String payload = http.getString();
  http.end();
  JsonObject& root = jsonBuffer.parseObject(payload);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    tpLinkCloudConnection = tpFailedToConnect;
    return;
  } else {
    const char* charToken = root["deviceId"];
    tpLinkLampDeviceId = String(charToken);
    tpLinkCloudConnection = tpConnected;
    Serial.println("Got tp link device id and connected to the tp link cloud");
  }
}
