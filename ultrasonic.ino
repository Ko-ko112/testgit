#include "ESP8266WiFi.h"
#include "PubSubClient.h"
// WiFi
const char* ssid = "chompoo"; //สร้างตัวแปรไว้เก็บชื่อ ssid ของ AP ของเรา
const char* pass = "0924314347"; //สร้างตัวแปรไว้เก็บชื่อ password ของ AP ของเรา

// MQTT
const char* mqtt_server = "172.20.10.2";  // IP of the MQTT broker
const char* C4Slot1_topic = "c4/slot1";
const char* C4Slot2_topic = "c4/slot2";
const char* mqtt_username = "car"; // MQTT username
const char* mqtt_password = "112233"; // MQTT password
const char* clientID = "client_livingroom"; // MQTT client ID

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient); 

int echo1 = D1;
int trig1 = D2;

int echo2 = D5;
int trig2 = D6;

long duration , cm;

// Custom function to connet to the MQTT broker via WiFi
void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, pass);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }

}

void setup() {
 Serial.begin(115200);
}
 

void loop(){

  connect_MQTT();
  Serial.setTimeout(2000);
  
  long slot1 = getLength(echo1, trig1); // วัดระยะทาง Ultrasonic ตัวที่ 1
  long slot2 = getLength(echo2, trig2); // วัดระยะทาง Ultrasonic ตัวที่ 2
  // .
  // .
  // .
  // long uN = getLength(echoN, trigN); // วัดระยะทาง Ultrasonic ตัวที่ N
  
  Serial.print("slot 1: ");
  if(cm > 151){
    Serial.print("[Empty].");
    }else{
      Serial.print("[Used].");
      }

        if(cm < 150 && cm >111){
          Serial.print("[small car].");
          Serial.print(slot1);
          Serial.print(" cm ");
          }
        if(cm < 110 && cm >70){
          Serial.print("[medium car].");
          Serial.print(slot1);
          Serial.print(" cm ");
          }
        if(cm <69){
          Serial.print("[large car].");
          Serial.print(slot1);
          Serial.print(" cm ");
          }
    Serial.println("  ");
// MQTT can only transmit strings
String c4s1="S1: "+String((long)slot1)+" cm ";
 // PUBLISH to the MQTT Broker (topic = Temperature, defined at the beginning)
  if (client.publish(C4Slot1_topic, String(slot1).c_str())) {
    Serial.println("Slot1 sent!");
  }
 // Again, client.publish will return a boolean value depending on whether it succeeded or not.
 // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("Slot1 failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(C4Slot1_topic, String(slot1).c_str());
  }
delay(100);

  Serial.print("slot 2: "); 
  if(cm > 151){
    Serial.print("[Empty].");
    }else{
      Serial.print("[Used].");
      }

       
        if(cm < 150 && cm >111){
          Serial.print("[small car].");
          Serial.print(slot2);
          Serial.print(" cm ");
          }
        if(cm < 110 && cm >70){
          Serial.print("[medium car].");
          Serial.print(slot2);
          Serial.print(" cm ");
          }
        if(cm <69){
          Serial.print("[large car].");
          Serial.print(slot2);
          Serial.print(" cm ");
          }
  
  // MQTT can only transmit strings
    String c4s2="S2: "+String((long)slot2)+" cm ";
  
  Serial.println("  ");
  delay(100);
  
  // PUBLISH to the MQTT Broker (topic = Humidity, defined at the beginning)
  if (client.publish(C4Slot2_topic, String(slot2).c_str())) {
    Serial.println("Slot2 sent!");
  }
  // Again, client.publish will return a boolean value depending on whether it succeeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("Slot2 failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(C4Slot2_topic, String(slot2).c_str());
  }

  client.disconnect();  // disconnect from the MQTT broker
  delay(2000);       // print new values every 1 Minute

}


long getLength(int echo, int trig){
  pinMode(trig, OUTPUT);
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(5);
  digitalWrite(trig, LOW);
  pinMode(echo, INPUT);
  duration = pulseIn(echo, HIGH);
  cm = microsecondsToCentimeters(duration);
  cm = constrain(cm, 4, 280);
  delay(100);
  return cm;
}

long microsecondsToCentimeters(long microseconds){
  // ความเร็วเสียงเดินทางในอากาศคือ 340 เมตร/วินาที หรือ 29 ไมโครวินาที ต่อเซนติเมตร
  // เวลาที่ Ping ออกไปกระทบวัตถุจนถึงกลับมาที่ตัวรับ ใช้หาระยะทางได้
  // วัตถุอยู่มีระยะทางเท่ากับครึ่งหนึ่งของเวลาที่จับได้
  // เขียนเป็นสมการคำนวนระยะทาง ได้ดังนี้
  return microseconds / 29 / 2;
}
