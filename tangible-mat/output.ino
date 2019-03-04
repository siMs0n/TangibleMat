void toggleMusic() {
  
}

void changeVolume() {
  //Remember to add debounce for volume (don't change it many times per second)
}

void toggleCoffee() {
  
}

void toggleIotLamp() {
  int newLampState;
  if(matState.iotLampState == 0) {
    newLampState = 1;
  } else {
    newLampState = 0;
  }

  Serial.println("Toggle tp link lamp via wifi");
  StaticJsonBuffer<200> jsonBuffer;
  
  HTTPClient http;
  http.begin("https://wap.tplinkcloud.com?token=" + tpLinkToken);
  http.addHeader("Content-Type", "application/json");
  String requestData = "{\"system\":{\"set_relay_state\":{\"state\":" + String(newLampState) + "}}}";
  int httpCode = http.POST("{\"method\": \"passthrough\", \"params\": {\"deviceId\": \"" + tpLinkLampDeviceId +", \"requestData\": " + requestData + "}}");
  Serial.println("Get device list post request http code: " + httpCode);
  String payload = http.getString();
  http.end();
  JsonObject& root = jsonBuffer.parseObject(payload);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    tpLinkCloudConnection = tpFailedToConnect;
    return;
  } else {
    //Hopefully succeeded
    matState.iotLampState = newLampState;
    Serial.println("Toggled iot lamp state");
  }
}

void dimIotLamp(int brightness) {

}
