

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

  RelayState = !RelayState;
  digitalWrite(Relay_Pin, RelayState);

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

