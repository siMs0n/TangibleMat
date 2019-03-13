#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

int irAnalogPin = A0;
int activationAreaLeftButtonPin = D1;
int activationAreaRightButtonPin = D2;
int coffeeButtonPin = D3;
int musicButtonPin = D4;
int iotLampButtonPin = D5;
int nightLightButtonPin = D6;

int nightLightOutputPin = D7;
int lightstripOutputPin = D8;

int nightLightLed = 0;
int iotLampLed = 1;
int musicLed = 25;
int coffeeLed = 26;

struct RawInput{ //Raw input, for example if buttons are HIGH or LOW
  boolean musicButton;
  boolean coffeeButton;
  boolean iotLampButton;
  boolean nightLightButton;
  boolean activationAreaLeftButton;
  boolean activationAreaRightButton;
  int sliderValue; //Value between 0-1023
};

struct ProcessedInput { //Processed input with logic. For example true/false if buttons are pressed.
  boolean musicButtonPressed;
  boolean musicButtonHold;
  boolean coffeeButtonPressed;
  boolean iotLampButtonPressed;
  boolean iotLampButtonHold;
  boolean nightLightButtonPressed;
  boolean activationAreaLeftButtonPressed;
  boolean activationAreaLeftButtonHold;
  boolean activationAreaRightButtonPressed;
  boolean activationAreaRightButtonHold;
  int sliderValue;
};

struct MatState { //For example is the lamp on, what volume, progress of coffee
  int iotLampState; // on/off 0/1
  int iotLampBrightness; //0-100
  int musicState;
  int volume;
  int nightLightState; // On/off 0/1
  int matState;
  int alarmState;
  int coffeeState;
};

RawInput lastCycleRawInput = {
  musicButton: false,
  coffeeButton: false,
  iotLampButton: false,
  nightLightButton: false,
  activationAreaLeftButton: false,
  activationAreaRightButton: false,
  sliderValue: 0 
};

RawInput currentRawInput = {
  musicButton: false,
  coffeeButton: false,
  iotLampButton: false,
  nightLightButton: false,
  activationAreaLeftButton: false,
  activationAreaRightButton: false,
  sliderValue: 0  
};

ProcessedInput processedInput = { //Processed input with logic. For example true/false if buttons are pressed. Creates the object 
  musicButtonPressed: false,
  musicButtonHold: false,
  coffeeButtonPressed: false,
  iotLampButtonPressed: false,
  iotLampButtonHold: false,
  nightLightButtonPressed: false,
  activationAreaLeftButtonPressed: false,
  activationAreaLeftButtonHold: false,
  activationAreaRightButtonPressed: false,
  activationAreaRightButtonHold: false,
  sliderValue: 0
};

MatState matState = {
  iotLampState: 0,
  iotLampBrightness: 10,
  musicState: 0,
  volume: 20,
  nightLightState:0,
  matState:0,
  alarmState:0,
  coffeeState: 0
};

unsigned long musicButtonStartHold = 0; //Timestamp for when music button hold was started
unsigned long iotLampButtonStartHold = 0;
unsigned long activationAreaButtonStartHold = 0;

unsigned long lastDebounceTimeSlider = 0;  // the last time the output pin was toggled
unsigned long debounceDelaySlider = 100;    // the debounce time; increase if the output flickers

int tpNotConnected = 0;
int tpConnecting = 1;
int tpConnected = 2;
int tpFailedToConnect = 3;
int tpLinkCloudConnection = tpNotConnected;
String tpLinkToken = "b5cf28b6-B7FOFHT0qcRgE1sGneBneWD";
String tpLinkLampDeviceId = "801228A381E22A6B1D817D2EAF09CFCA18CAC9E6";
String tpLinkFingerPrint = "39:E1:F0:E9:5E:41:3D:97:F7:BF:81:82:0C:F8:ED:41:E0:96:AF:DA";
String laptopIp = "192.168.1.102";

void setup() {
  pinMode(coffeeButtonPin, INPUT);
  pinMode(musicButtonPin, INPUT);
  pinMode(iotLampButtonPin, INPUT);
  pinMode(nightLightButtonPin, INPUT);
  pinMode(activationAreaLeftButtonPin, INPUT);
  pinMode(activationAreaRightButtonPin, INPUT);

  pinMode(irAnalogPin, INPUT);

  pinMode(nightLightOutputPin, OUTPUT);

  Serial.begin(9600);
  Serial.println("Setting up mat");
  initLedStrips();
  ledCoolAnimate(); //Remove later
  connectToWifi();
}

void loop() {
  if(WiFi.status() == WL_CONNECTED && tpLinkCloudConnection == tpNotConnected){
    Serial.println("Connected to wifi");
    //loginToIotLampCloud();
    tpLinkCloudConnection = tpConnected;
  }
  //readMatInput();
  //processMatInput();
  if ((millis() - lastDebounceTimeSlider) > debounceDelaySlider) {
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

    if(processedInput.iotLampButtonHold) {
      dimIotLamp();
    }
    
    if(processedInput.nightLightButtonPressed) {
      toggleNightLight();
    }
    
    if(processedInput.activationAreaLeftButtonPressed) {
      //Do we have a function we want to do here?
    }
  
    if(processedInput.activationAreaLeftButtonHold || processedInput.activationAreaRightButtonHold) {
      if(matState.matState == 0){
        activateMat();
      }
      if(matState.alarmState == 1){
        turnOffAlarm();
      }
    }
     
    if(processedInput.activationAreaRightButtonPressed) {
      //Do we have a function we want to do here?
    }
    lastDebounceTimeSlider = millis();
  }
}

void readMatInput() {
  currentRawInput = {
    musicButton: digitalRead(musicButtonPin),
    coffeeButton: digitalRead(coffeeButtonPin),
    iotLampButton: digitalRead(iotLampButtonPin),
    nightLightButton: digitalRead(nightLightButtonPin),
    activationAreaLeftButton: digitalRead(activationAreaLeftButtonPin),
    activationAreaRightButton:digitalRead(activationAreaRightButtonPin),
    sliderValue: analogRead(irAnalogPin)   
  };
}

void processMatInput() {
  processedInput = {
    musicButtonPressed: false,
    musicButtonHold: false,
    coffeeButtonPressed: false,
    iotLampButtonPressed: false,
    iotLampButtonHold: false,
    nightLightButtonPressed: false,
    activationAreaLeftButtonPressed: false,
    activationAreaLeftButtonHold: false,
    activationAreaRightButtonPressed: false,
    activationAreaRightButtonHold: false,
    sliderValue: 0    
  };
  //Serial.println("Raw input: " + currentRawInput.sliderValue);
  
  //Music button
  if(lastCycleRawInput.musicButton == HIGH && currentRawInput.musicButton == LOW) {
    processedInput.musicButtonPressed = true;
  } else if(lastCycleRawInput.musicButton == LOW && currentRawInput.musicButton == HIGH) {
    musicButtonStartHold = millis();
  } else if(lastCycleRawInput.musicButton == HIGH && currentRawInput.musicButton == HIGH &&
        millis() - musicButtonStartHold > 1000) {
    processedInput.musicButtonHold = true;
  }


  //Coffee button
  if(lastCycleRawInput.coffeeButton == HIGH && currentRawInput.coffeeButton == LOW) {
    processedInput.coffeeButtonPressed = true;
  }

  //Iot lamp button
  if(lastCycleRawInput.iotLampButton == HIGH && currentRawInput.iotLampButton == LOW) {
    processedInput.iotLampButtonPressed = true;
  } else if(lastCycleRawInput.iotLampButton == LOW && currentRawInput.iotLampButton == HIGH) {
    iotLampButtonStartHold = millis();
  } else if(lastCycleRawInput.iotLampButton == HIGH && currentRawInput.iotLampButton == HIGH &&
        millis() - iotLampButtonStartHold > 1000) {
    processedInput.iotLampButtonHold = true;
  }

//nightLight
  if(lastCycleRawInput.nightLightButton == HIGH && currentRawInput.nightLightButton == LOW) {
    processedInput.nightLightButtonPressed = true;
  }

  boolean lastCycleHigh = lastCycleRawInput.activationAreaLeftButton == HIGH || 
  lastCycleRawInput.activationAreaRightButton == HIGH;
  boolean currentCycleHigh = currentRawInput.activationAreaLeftButton == HIGH ||
  currentRawInput.activationAreaRightButton == HIGH;
  boolean lastCycleLow = lastCycleRawInput.activationAreaLeftButton == LOW && 
  lastCycleRawInput.activationAreaRightButton == LOW;
   
 //activtionArea
  if(lastCycleLow && currentCycleHigh) {
    activationAreaButtonStartHold = millis();
  } else if(lastCycleHigh && currentCycleHigh && millis() - activationAreaButtonStartHold > 1000 ){
    processedInput.activationAreaLeftButtonHold = true;
    processedInput.activationAreaRightButtonHold = true; 
  }
  
 
  //Slider
  int outputValue = map(currentRawInput.sliderValue, 0, 1023, 255, 0);
  
  outputValue = outputValue - 15;
  if( outputValue < 0 ) {
    processedInput.sliderValue = 0;
  } else if(outputValue > 160) {
    processedInput.sliderValue = 100;
  } else {
    processedInput.sliderValue = outputValue * (100.0/160.0);
  }

  //Serial.println("outputValue: " + String(outputValue));
  //Serial.println("Slidervalue: " + String(processedInput.sliderValue));

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
  http.begin("https://wap.tplinkcloud.com", tpLinkFingerPrint);
  http.addHeader("Content-Type", "application/json");
  String loginRequest = "{\"method\": \"login\",\"params\": {\"appType\": \"Kasa_Android\",\"cloudUserName\": \"stina.vk@gmail.com\",\"cloudPassword\": \"Tangible\",\"terminalUUID\": \"015ea6b4-26f4-4d50-af6f-d543024f57b8\"}}";
  Serial.println("Login request:");
  Serial.println(loginRequest);
  int httpCode = http.POST(loginRequest);
  Serial.println("Login post request http code: " + String(httpCode));
  String payload = http.getString();
  Serial.println(payload);
  http.end();
  //char json[1000]; 
  //payload.toCharArray(json, 1000);
  JsonObject& root = jsonBuffer.parseObject(payload);
  if (!root.success()) {
    Serial.println("parseObject() failed at login");
    tpLinkCloudConnection = tpFailedToConnect;
    fetchTpLinkDeviceId();
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
  http.begin("https://wap.tplinkcloud.com?token=" + tpLinkToken, tpLinkFingerPrint);
  http.addHeader("Content-Type", "application/json");
  String deviceListRequest = "{\"method\": \"getDeviceList\"}";
  int httpCode = http.POST(deviceListRequest);
  Serial.println("Get device list post request http code: " + String(httpCode));
  String payload = http.getString();
  http.end();
  JsonObject& root = jsonBuffer.parseObject(payload);
  if (!root.success()) {
    Serial.println("parseObject() failed at get device id");
    tpLinkCloudConnection = tpFailedToConnect;
    return;
  } else {
    const char* charToken = root["deviceId"];
    tpLinkLampDeviceId = String(charToken);
    tpLinkCloudConnection = tpConnected;
    Serial.println("Got tp link device id and connected to the tp link cloud");
  }
}
