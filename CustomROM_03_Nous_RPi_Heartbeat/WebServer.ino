

void WebServer_Config()
{
  
  server.onNotFound(handleNotFound);
  server.on("/",Webpage_Home1);
  server.on("/OTA",Webpage_OTA);
  server.on("/restart",Webpage_restart);
  server.on("/on",Webpage_turn_on);
  server.on("/off",Webpage_turn_off);

  server.begin();  
}

void Webpage_Home1()
{
  Prep_webpage1();
  server.send ( 200, "text/html", webpage1 );
  delay(10);
  webpage1 = "";
}

void Webpage_OTA()
{
  OTA_Mode = 1;
  server.send(200,"text/plain","OTA mode active now");
}


void handleNotFound()
{
  server.send(404, "text/plain", "404: Not found! Send one of these: /OTA, /restart, /webpage1, /on, /off");
}

void Webpage_restart()
{
  server.send(200,"text/plain","Restarting ESP");
  Serial.println("Restart requested from webserver");
  delay(1000);
  Restart_ESP();
}

void Webpage_turn_on()
{
  Turn_LED_On();

  RelayState = 1;
  digitalWrite(Relay_Pin, RelayState);
  
  unsigned long epochTime = timeClient.getEpochTime();
  String formattedTime = timeClient.getFormattedTime();
  String currentDateTime = String(epochTime) + " :: " + formattedTime;

  RPi_last_restart_time = currentDateTime;
  RPi_restart_millis = millis()/1000;
  RPi_restart_count = RPi_restart_count +100000 ;

  MQTT_heartbeat();

  server.send(200,"text/plain","Turned on");
}

void Webpage_turn_off()
{
  Turn_LED_On();

  RelayState = 0;
  digitalWrite(Relay_Pin, RelayState);

  MQTT_heartbeat();

  server.send(200,"text/plain","Turned off");
}
