void toggleMusic() {
  Serial.println("Toggle music");
}

void changeVolume() {
   if ((millis() - lastDebounceTimeSlider) > debounceDelaySlider) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    matState.volume = processedInput.sliderValue;
    Serial.println("Change volume");
     //Send volume change

     
    lastDebounceTimeSlider = millis();
   }
}

void toggleCoffee() {
  
}

void toggleNightLight() {
  Serial.println("Toggle night light");
  if(matState.nightLightState == 1) {
    digitalWrite(nightLightOutputPin, LOW);
    matState.nightLightState = 0;
  } else {
    digitalWrite(nightLightOutputPin, HIGH);
    matState.nightLightState = 1;
  }
}

void activateMat(){
  Serial.println("Activating mat");
  ledCoolAnimate();
  matState.matState = 1; 
}

void turnOffAlarm(){
  matState.alarmState = 0;
  //Send to alarm to turn off
}

void toggleIotLamp() {
  int newLampState;
  if(matState.iotLampState == 0) {
    newLampState = 1;
  } else {
    newLampState = 0;
  }

  

  Serial.println("Toggle tp link lamp via wifi");
  HTTPClient http;
  String laptopIp = "192.168.1.101";
  http.begin("http://" + laptopIp + ":3000/set-state");
  http.addHeader("Content-Type", "application/json");
  Serial.println("Dim request:");
  String request = "{\"state\": " + String(newLampState) + ", \"brightness\": 50}";
  Serial.println(request);
  int httpCode = http.POST(request);
  //int httpCode = http.POST("{state: 1, brightness: 20}");
  Serial.println("Toggle http code: " + String(httpCode));
  String payload = http.getString();
  Serial.println("Response: " + payload);
  matState.iotLampState = newLampState;
  Serial.println("Toggled iot lamp state");
  http.end();
  /*StaticJsonBuffer<200> jsonBuffer;
  
  HTTPClient http;
  String url = "https://wap.tplinkcloud.com?token=" + tpLinkToken;
  //String url = "https://wap.tplinkcloud.com";
  Serial.println("url:" + url);
  Serial.println("tpLinkFingerPrint: " + tpLinkFingerPrint);
  //http.begin(url, tpLinkFingerPrint);
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  String requestData = "{\"smartlife.iot.smartbulb.lightingservice\":{\"transition_light_state\":{\"brightness\":20,\"on_off\":0}}}";
  String request = "{method: \"passthrough\", params: {deviceId: \"" + tpLinkLampDeviceId +"\", requestData: '" + requestData + "'}}";
  Serial.println("Toggle request:");
  Serial.println(request);
  int httpCode = http.POST(request);
  Serial.println("Toggle http code: " + String(httpCode));
  String payload = http.getString();
  Serial.println(payload);
  http.end();
  JsonObject& root = jsonBuffer.parseObject(payload);
  if (!root.success()) {
    Serial.println("parseObject() failed at toggle lamp");
    tpLinkCloudConnection = tpFailedToConnect;
    return;
  } else {
    //Hopefully succeeded
    matState.iotLampState = newLampState;
    Serial.println("Toggled iot lamp state");
  }*/
}

void dimIotLamp() {
  if ((millis() - lastDebounceTimeSlider) > debounceDelaySlider) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    Serial.println("Dim light");

    HTTPClient http;
    String laptopIp = "192.168.1.101";
    http.begin("http://" + laptopIp + ":3000/set-state");
    http.addHeader("Content-Type", "application/json");
    Serial.println("Dim request:");
    String request = "{\"state\": 1, \"brightness\": " + String(processedInput.sliderValue) + "}";
    Serial.println(request);
    int httpCode = http.POST(request);
    //int httpCode = http.POST("{state: 1, brightness: 20}");
    Serial.println("Toggle http code: " + String(httpCode));
    String payload = http.getString();
    Serial.println("Response: " + payload);
    http.end();

    matState.volume = processedInput.sliderValue;
    lastDebounceTimeSlider = millis();
   }
}
