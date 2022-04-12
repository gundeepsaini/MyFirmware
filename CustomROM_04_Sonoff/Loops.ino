

void Always_loop()
{
  OTA_Check();
}


void Fast_Loop()
{   
  server.handleClient();  
    
  if(Button_Pressed)
    ButtonPress_Actions();  

  MQTT_loop();
}




void Mid1_Loop()
{  
  Turn_LED_Off();
}



void Mid2_Loop()
{
  
}

void Slow_Loop()
{    
  MQTT_publish();  

  if(!Time_NTP_isValid())
      Time_NTP_Update();
}

void VerySlow_Loop()
{
  Time_NTP_Update();
}
