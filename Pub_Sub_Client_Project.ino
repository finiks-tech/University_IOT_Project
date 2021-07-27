#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2
#define relay1 5	//define relays
#define relay2 16

const char* ssid = "admin";	//your wifi ssid
const char* password = "********";

const char* mqtt_server = "192.168.1.105"; //your mqtt Server addres

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

WiFiClient IRremot;
PubSubClient client(IRremot);

const uint16_t kIrLed = 14;
IRsend irsend(kIrLed);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messagein;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messagein += (char)message[i];
  }
//-----------------------------------------
//**********************
if(topic=="home/IR"){		//check topics and payloads message
    if (messagein == "PO"){ 
    irsend.sendNEC(0xFDC03F);	//send IR hex code
      Serial.println("POWER");
  }else if (messagein == "VU"){
    irsend.sendNEC(0xFD6897);
      Serial.println("VU");
  }else if (messagein == "VD") {
    irsend.sendNEC(0xFD58A7);
      Serial.println("VD");
  }else if (messagein == "CD") {
    irsend.sendNEC(0xFD18E7);
      Serial.println("CD");
  }else if (messagein == "CU") {
    irsend.sendNEC(0xFD28D7);
      Serial.println("CU");
  }else if (messagein == "UP"){
    irsend.sendNEC(0xFDB847);
      Serial.println("up");
  }else if(messagein == "DOWN"){
    irsend.sendNEC(0xFDA25D);
      Serial.println("down");  
  }else if(messagein == "LEFT"){
    irsend.sendNEC(0xFD02FD);
      Serial.println("left");
  }else if(messagein == "RIGHT"){
    irsend.sendNEC(0xFD42BD); 
      Serial.println("right"); 
  }else if(messagein == "HOME"){
    irsend.sendNEC(0xFD8877); 
      Serial.println("home");
  }else if(messagein == "EXIT"){
    irsend.sendNEC(0xFD22DD); 
      Serial.println("exit");  
  }else if(messagein == "MENU"){
  irsend.sendNEC(0xFD38C7); 
      Serial.println("menu");
  }else if(messagein == "ENTER"){
    irsend.sendNEC(0xFD827D); 
      Serial.println("enter");
    }else if(messagein == "MUTE"){
      irsend.sendNEC(0xFDA857); 
      Serial.println("mute");
      }
    }
//+++++++++++++++++++++++++
if(topic=="home/lamp"){
    if (messagein == "R1ON"){ 
      digitalWrite(relay1 , HIGH) ;
      Serial.println("L_ON");
      
      client.publish("home/lamp","TV POWER IS ON");
  
  }else if ( messagein == "R1OFF") {
    digitalWrite(relay1 ,LOW) ;
    Serial.println("L_off");
 
      client.publish("home/lamp","TV POWER IS OFF");
  }
}
//+++++++++++++++++++++++++++++

    if (messagein == "R2ON"){ 
      digitalWrite(relay2 , HIGH) ;
      Serial.println("L_ON");
      
      client.publish("home/lamp","lamp is ON");
  
  }else if ( messagein == "R2OFF") {
    digitalWrite(relay2 ,LOW) ;
    Serial.println("L_off");
 
      client.publish("home/lamp","lamp is OFF");
  }



  //+++++++++++++++++++++++++++
}
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
 
   //  String clientId = "ESP8266Client-";
   // clientId += String(random(0xffff), HEX);
    // Attempt to connect
   // if (client.connect(clientId.c_str())) {
           if (client.connect("IRrm")) {

      Serial.println("connected");  
      client.subscribe("home/IR");
      client.subscribe("home/lamp");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(115200);
  pinMode( relay1 , OUTPUT);
  digitalWrite(relay1 , LOW) ;
  pinMode( relay2 , OUTPUT);
  digitalWrite(relay2 , LOW) ;
  sensors.begin();
  irsend.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("IRrm");

 sensors.requestTemperatures();
 int tempC = sensors.getTempCByIndex(0);
 if(tempC != DEVICE_DISCONNECTED_C) 
  {
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(tempC);
     char tempC_c [8];
      dtostrf(tempC,6,2,tempC_c);
    client.publish("home/temp",tempC_c);
  } 
  else
  {
    Serial.println("Error: Could not read temperature data");
  } 
    
         //  char hh [8];
         //  dtostrf(h,6,2,hh);
         //  char tt [8];
         //  dtostrf(t,6,2,tt);
      
        //client.publish("IR/Power","Poweroff");
      //  client.publish("device/alh",hh);

  delay(100);
  }
 
