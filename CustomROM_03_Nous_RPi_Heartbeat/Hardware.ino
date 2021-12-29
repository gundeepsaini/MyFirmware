

void Hardware_Config()
{
  pinMode(Button_Pin, INPUT);

  pinMode(LED_Pin_Red, OUTPUT);
  pinMode(LED_Pin_Blue, OUTPUT);
  pinMode(Relay_Pin, OUTPUT);  
}

void Hardware_Defaults()
{
  // default relay state
  RelayState = 1;
  digitalWrite(Relay_Pin, RelayState);

  digitalWrite(LED_Pin_Red, HIGH);  // red off

}



void Hardware_Interupt_Config()
{
  attachInterrupt(digitalPinToInterrupt(Button_Pin), Handle_ButtonPress, FALLING);    // Active low
}


ICACHE_RAM_ATTR void Handle_ButtonPress()
{ 
  //Serial.println("Button Press");
  Button_Pressed = 1;
}


long MotionOn_last_timeStamp, MotionOff_last_timeStamp;
bool last_montionState;


void ButtonPress_Actions()
{
  Button_Pressed = 0;
  Turn_LED_On();

  // Disabled to diagnose false triggers
  //RelayState = !RelayState;
  //digitalWrite(Relay_Pin, RelayState);

  unsigned long epochTime = timeClient.getEpochTime();
  String formattedTime = timeClient.getFormattedTime();
  RPi_last_restart_time = String(epochTime) + " :: " + formattedTime;
  
  RPi_restart_millis = millis()/1000;
  RPi_restart_count = RPi_restart_count +100 ;

  MQTT_heartbeat();
}




void Turn_LED_On()
{
  digitalWrite(LED_Pin_Red, LOW);
  LED_OneTiming = millis()/1000;
}


void Turn_LED_Off()
{
  if(millis()/1000 - LED_OneTiming > 1) // in seconds
  {
    digitalWrite(LED_Pin_Red, HIGH);
    LED_OneTiming = 0;
  }
}





void Check_RPi_Heartbeat()
{
  if(RPi_heartbeat_Enable == 1)
  {
    if(millis()/1000 - RPi_msg_millis > RPi_heartbeat_restart_max_time)
    {
      // Message not recd from RPi for a set time.
      // Switch off RPi, wait, and switch on

      if(millis()/1000 - RPi_restart_millis > 1 * 60 * 60 || RPi_restart_millis == 0)     // restart only if last restart was ___ hour ago
      {
          RPi_restart_millis = millis()/1000;
         
          unsigned long epochTime = timeClient.getEpochTime();
          String formattedTime = timeClient.getFormattedTime();
         
          RPi_restart_count = RPi_restart_count +1 ;

          digitalWrite(LED_Pin_Red, LOW);    
          RPi_last_restart_time = String(epochTime) + " :: " + formattedTime;
          RelayState = 0;
          digitalWrite(Relay_Pin, RelayState);

          delay(5000);

          digitalWrite(LED_Pin_Red, HIGH);    
          RelayState = 1;
          digitalWrite(Relay_Pin, RelayState);
        }
    }
  }
}


          

          
