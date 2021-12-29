 /*-----------------------------------------

            Nous smart plug module - Custom ROM

Date: Nov 2021 (v01)
Device: ESP8285 - Nous A1
  - It is based on Tuya devices
  - Similar to Gosund SP111 v1.1 Power Monitoring Plug
  - Pin config and details: https://templates.blakadder.com/gosund_SP111_v1_1
  - Check pin and PCB layout in images

Description:
  - Hacked Nous module for device control
  - MQTT, Web Interface supported
  - OTA support


Note:
  - GPIO2 has to be grounded before powerup first Arduino non-OTA prog
    only applicable if programming via serial (not OTA)

------------------------------------------- */


/* ------------- LIB ------------------------------ */

#include "Secrets.h"

#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>



/* ------------- CONFIG VAR ------------------------------ */
unsigned long looptime_Fast = 0;    // in secs
unsigned long looptime_Mid1 = 1;    // in secs
unsigned long looptime_Mid2 = 5;   // in secs
unsigned long looptime_Slow       = 1  * (60);      // in mins
unsigned long looptime_VerySlow   = 30 * (60);      // in mins

int RPi_heartbeat_restart_max_time = 15 * 60;      // in minutes

#define LED_Pin_Red     0   // GPIO00 inverted
#define LED_Pin_Blue    2   // GPIO02 inverted   (LED_BUILTIN desginated)
#define Button_Pin      13  // GPIO13 inverted   (Active low)
#define Relay_Pin       15  // GPIO15

// For power measurement?
#define HLWBL_CF1       4
#define BL0937_CF       5
#define HLWBL_SELi      12


/* ------------- VAR ------------------------------ */
const char* ssid             = SECRET_WIFI_SSID3;
const char* pass             = SECRET_WIFI_PASS3;
const char* DeviceHostName   = SECRET_Device_Name11;
const char* OTA_Password     = SECRET_Device_OTA_PASS; 
unsigned long lastrun_fast, lastrun_Mid1;
unsigned long lastrun_Mid2, lastrun_slow, lastrun_Veryslow;
unsigned long lastrun_30mins,lastrun_OTA;
bool OTA_Mode=0;
String webpage1="";

ESP8266WebServer server(80);


bool Button_Pressed;
long LED_OneTiming = 0;
bool RelayState = 0;
long MQTT_PIR_heartbeat_timestamp, MQTT_PIR_last_ON_msg_timestamp;

int RPi_msg = 0;
int RPi_heartbeat_Enable  = 1;      // Default on - always monitoring, restart after 15 mins of no response from RPi
int RPi_restart_count = 0;
unsigned long RPi_msg_millis;
unsigned long RPi_restart_millis;

String RPi_startup_time;
String RPi_msg_time;
String RPi_last_restart_time;


void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Hardware_Config();
  Hardware_Defaults();
  
  Serial.begin(115200);
  Serial.println(DeviceHostName);

  wifi_station_set_hostname(DeviceHostName);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) 
  {
    Serial.println("Wifi Error! Rebooting in 30s...");
    delay(30 *1000);
    digitalWrite(LED_BUILTIN, HIGH);
    Restart_ESP();
  }
  
  Serial.print("Connected to ");
  Serial.print(WiFi.SSID());
  Serial.print(", IP Address: ");
  Serial.println(WiFi.localIP());
  
  
  OTA_Config();
  WebServer_Config();
  Hardware_Interupt_Config();
  MQTT_Config();
  Config_Time(); 
  
  
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Ready");
}


void loop() 
{
  // Always
  Always_loop();

  // Fast Loop
 if(!OTA_Mode && ((millis()/1000 - lastrun_fast > looptime_Fast) || lastrun_fast ==0))
  {
    lastrun_fast = millis()/1000;
    Fast_Loop();
  }


  // Mid1 Loop
 if(!OTA_Mode && ((millis()/1000 - lastrun_Mid1 > looptime_Mid1) || lastrun_Mid1 ==0))
  {
    lastrun_Mid1 = millis()/1000;
    Mid1_Loop();
  }

  // Mid2 Loop
 if(!OTA_Mode && ((millis()/1000 - lastrun_Mid2 > looptime_Mid2) || lastrun_Mid2 ==0))
  {
    lastrun_Mid2 = millis()/1000;
    Mid2_Loop();
  }

  // Slow Loop
 if(!OTA_Mode && ((millis()/1000 - lastrun_slow > looptime_Slow) || lastrun_slow ==0))
  {
    lastrun_slow = millis()/1000;
    Slow_Loop();
  }

    // Very Slow Loop
 if(!OTA_Mode && ((millis()/1000 - lastrun_Veryslow > looptime_VerySlow) || lastrun_Veryslow ==0))
  {
    lastrun_Veryslow = millis()/1000;
    VerySlow_Loop();
  }

}



void Restart_ESP()
{
  Serial.println("Restarting ESP");
  ESP.restart();
  delay(1000);
  while(1);
}
