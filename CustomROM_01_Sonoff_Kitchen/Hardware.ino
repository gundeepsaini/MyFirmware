

void Hardware_Config()
{
  pinMode(Button_Pin, INPUT);
  pinMode(PIR_Pin, INPUT);

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


long MotionOn_last_timeStamp, MotionOff_last_timeStamp;
bool last_montionState;

void Motion_Detected_Actions()
{
  //Motion_Detected = 0;
  last_montionState = MotionState;
  MotionState = !digitalRead(PIR_Pin);

  if(MotionState != last_montionState)  // only execute if motion state has changed since last time
  {
    if(MotionState)
    {
      Turn_LED_On();

      //if(millis()/1000 - MotionOn_last_timeStamp > 10)
      {
        // Debounce protection - prevent MQTT publish / relay switching if signal changes <10sec
        MotionOn_last_timeStamp = millis()/1000;
        if(Motion_Control)
        {
          RelayState = 1;
          digitalWrite(Relay_Pin, RelayState);
        }

        MQTT_heartbeat();
      }

    }
    else
    {
      //if(millis()/1000 - MotionOff_last_timeStamp > 30)
      {
        // Debounce protection - prevent MQTT publish / relay switching if signal changes <10sec
        MotionOff_last_timeStamp = millis()/1000;
        if(Motion_Control)
        {
          RelayState = 0;
          digitalWrite(Relay_Pin, RelayState);
        }

        MQTT_heartbeat();
      }
    }
  }  
}


void ButtonPress_Actions()
{
  Button_Pressed = 0;
  Turn_LED_On();

  RelayState = !RelayState;
  digitalWrite(Relay_Pin, RelayState);
  MotionOn_last_timeStamp = millis()/1000;

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



void Motion_Detected_Actions_Updated()
{  
  // Read motion sensor state
  MotionState = !digitalRead(PIR_Pin);


  // Check if motion control is enabled
  if(Motion_Control)
  {

// relay is off and no motion is detected
      if(RelayState ==0 && MotionState ==0)
      {
        // do nothing
        int dummy = 0;
      }

// relay is off and motion is detected
      if(RelayState ==0 && MotionState ==1)
      {
        // Turn on relay - do it only once
        Turn_LED_On();
        RelayState = 1;
        digitalWrite(Relay_Pin, RelayState);
                
        MQTT_heartbeat();
      }

// relay is on and motion is detected
      if(RelayState ==1 && MotionState ==1)
      {
        // record last motion timestamp
        MotionOn_last_timeStamp = millis()/1000;
      }

// relay is on and no motion is detected
      if(RelayState ==1 && MotionState ==0)
      {
        // check if desired time has passed without any motion
         if(millis()/1000 - MotionOn_last_timeStamp > 10)
            {
              MotionOn_last_timeStamp = 0;                     
              RelayState = 0;
              digitalWrite(Relay_Pin, RelayState);
                      
              MQTT_heartbeat();        
            }          
      } 
  }
}
