

// https://pubsubclient.knolleary.net/api


/******************** LIB **************************************/
#define MQTT_MAX_PACKET_SIZE 1024  // < ----- Change in lib: This is because the defaul maxium length is 128b. So just go to PubSubClient.h and change #define MQTT_MAX_PACKET_SIZE 128 to #define MQTT_MAX_PACKET_SIZE 1024
#include "src/PubSubClient/PubSubClient.h"
//#include <ArduinoJson.h>


/********************* Var *************************************/
WiFiClient espClient;
PubSubClient client(espClient);

long lastReconnectAttempt = 0;

/******************** Config **************************************/
const char* mqtt_server     = SECRET_MQTT_Server;
const char* mqtt_user       = SECRET_MQTT_User;
const char* mqtt_password   = SECRET_MQTT_Pass;


// Light
#define MQTT_TOPIC_STATE_LIGHT "HA/Custom05_Sonoff/state" 
#define MQTT_TOPIC_CMD_LIGHT   "HA/Custom05_Sonoff/cmd"  

// Will Topic - Availability
#define MQTT_TOPIC_WILL        "HA/Custom05_Sonoff/status"     
#define MQTT_OFFLINE           "Offline"
#define MQTT_ONLINE            "Active"

#define MQTT_ON                "ON"
#define MQTT_OFF               "OFF"



/**************** External Functions ************************************/

void MQTT_Config()
{
	client.setServer(mqtt_server, SECRET_MQTT_Port);
 	client.setCallback(MQTT_MessageRecd_callback);  
}


void MQTT_loop()
{
	if (!client.connected())
    	MQTT_reconnect();              
  
  client.loop();
}


void MQTT_publish()
{   
    MQTT_heartbeat();
}


/**************** Internal Functions ************************************/


void MQTT_reconnect() 
{
  if (millis()/1000 - lastReconnectAttempt > 30 || lastReconnectAttempt == 0) 
  {
      Serial.println("MQTT reconnecting");
      
      //boolean connect (clientID, [username, password], [willTopic, willQoS, willRetain, willMessage], [cleanSession])
      if (client.connect(DeviceHostName, mqtt_user, mqtt_password, MQTT_TOPIC_WILL, 1, true, MQTT_OFFLINE)) 
      {
        //MQTT_publish_config();  
        Serial.println("MQTT connected");
        client.publish(MQTT_TOPIC_WILL, MQTT_ONLINE, true);
        client.subscribe(MQTT_TOPIC_CMD_LIGHT);
      }
      lastReconnectAttempt = millis()/1000;
  }
}


void MQTT_MessageRecd_callback(char* p_topic, byte* p_payload, unsigned int p_length) 
{
  String payload;
  for (uint8_t i = 0; i < p_length; i++) 
    { payload.concat((char)p_payload[i]); }

  if (String(MQTT_TOPIC_CMD_LIGHT).equals(p_topic)) 
  {
    int cmd = payload.toInt();
    Serial.print("MQTT CMD Light:");
    Serial.println(cmd);
    
    Turn_LED_On();
    RelayState = bool(cmd);
    digitalWrite(Relay_Pin, RelayState);

    MQTT_heartbeat();
  }
 

}


void MQTT_heartbeat()
{
  if(RelayState)
    client.publish(MQTT_TOPIC_STATE_LIGHT, MQTT_ON, true);
  else
    client.publish(MQTT_TOPIC_STATE_LIGHT, MQTT_OFF, true);   
 
}
