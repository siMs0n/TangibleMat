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

void dimIotLamp(int brightness) {
  HTTPClient http;
  String laptopIp = "192.168.1.101";
  http.begin("http://" + laptopIp + ":3000/set-state");
  http.addHeader("Content-Type", "application/json");
  Serial.println("Dim request:");
  String request = "{\"state\": 1, \"brightness\": " + String(brightness) + "}";
  Serial.println(request);
  int httpCode = http.POST(request);
  //int httpCode = http.POST("{state: 1, brightness: 20}");
  Serial.println("Toggle http code: " + String(httpCode));
  String payload = http.getString();
  Serial.println("Response: " + payload);
  http.end();
}
