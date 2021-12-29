

void Hardware_Config()
{
  pinMode(Button_Pin, INPUT);

  pinMode(LED_Pin, OUTPUT);
  pinMode(Relay_Pin, OUTPUT);  
}

void Hardware_Defaults()
{
  // default relay state
  RelayState = 1;
  digitalWrite(Relay_Pin, RelayState);
}



void Hardware_Interupt_Config()
{
  attachInterrupt(digitalPinToInterrupt(Button_Pin), Handle_ButtonPress, FALLING);          // Active low
  //attachInterrupt(digitalPinToInterrupt(PIR_Pin), Handle_Motion_On, FALLING);             // Active low - Motion detected true
  //attachInterrupt(digitalPinToInterrupt(PIR_Pin), Handle_Motion_Off, RISING);             // Active low - Motion detected false
}


/*
ICACHE_RAM_ATTR void Handle_Motion_On()
{
  Motion_Detected = 1;
  MotionState = 1;
}

ICACHE_RAM_ATTR void Handle_Motion_Off()
{
  Motion_Detected = 1;
  MotionState = 0;
}

*/

ICACHE_RAM_ATTR void Handle_ButtonPress()
{ 
  //Serial.println("Button Press");
  Button_Pressed = 1;
}



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
  digitalWrite(LED_Pin, LOW);
  LED_OneTiming = millis()/1000;
}


void Turn_LED_Off()
{
  if(millis()/1000 - LED_OneTiming > 1) // in seconds
  {
    digitalWrite(LED_Pin, HIGH);
    LED_OneTiming = 0;
  }
}


