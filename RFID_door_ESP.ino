/* IOT Project
 * RFID-enabled Door Lock System
 * By:
 * TANUMON ROY
 * B. Tech (3/4) - ECE - 174259
*/

#include "FS.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <MFRC522.h>
#include <BlynkSimpleEsp8266.h>

#define SS_PIN D4
#define RST_PIN D2
//#define openLED D0
//#define closeLED D1
//#define waitLED D8
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Update these with values suitable for your network.

const char* ssid = "*****";
const char* password = "*****";

#define BLYNK_AUTH "*****" //Blynk App Auth code

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

const char* AWS_endpoint = "*****"; //MQTT broker ip

void callback(char* topic, byte* payload, unsigned int length) {
Serial.print("Message arrived [");
Serial.print(topic);
Serial.print("] ");
for (int i = 0; i < length; i++) {
Serial.print((char)payload[i]);
}
Serial.println();

}
WiFiClientSecure espClient;
PubSubClient client(AWS_endpoint, 8883, callback, espClient); //set MQTT port number to 8883 as per //standard
//long lastMsg = 0;
//char msg[50];
//int value = 0;

void setup_wifi() {

delay(10);
// We start by connecting to a WiFi network
espClient.setBufferSizes(512, 512);
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);

WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}

Serial.println("");
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());

timeClient.begin();
while(!timeClient.update()){
timeClient.forceUpdate();
}

espClient.setX509Time(timeClient.getEpochTime());

}

void reconnect() {
// Loop until we're reconnected
while (!client.connected()) {
Serial.print("Attempting MQTT connection...");
// Attempt to connect
if (client.connect("ESPthing")) {
Serial.println("connected");
// Once connected, publish an announcement...
client.publish("outTopic", "hello world");
// ... and resubscribe
client.subscribe("inTopic");
} else {
Serial.print("failed, rc=");
Serial.print(client.state());
Serial.println(" try again in 5 seconds");

char buf[256];
espClient.getLastSSLError(buf,256);
Serial.print("WiFiClientSecure SSL error: ");
Serial.println(buf);

// Wait 5 seconds before retrying
delay(5000);
}
}
}

void setup() {

Blynk.begin(BLYNK_AUTH, ssid, password);
Serial.begin(115200);
Serial.setDebugOutput(true);
// initialize digital pin LED_BUILTIN as an output.
pinMode(LED_BUILTIN, OUTPUT);
pinMode(D3, OUTPUT);
digitalWrite(D3,LOW);
setup_wifi();
delay(1000);
if (!SPIFFS.begin()) {
Serial.println("Failed to mount file system");
return;
}

Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());

// Load certificate file
File cert = SPIFFS.open("/cert.der", "r"); //replace cert.crt eith your uploaded file name
if (!cert) {
Serial.println("Failed to open cert file");
}
else
Serial.println("Success to open cert file");

delay(1000);

if (espClient.loadCertificate(cert))
Serial.println("cert loaded");
else
Serial.println("cert not loaded");

// Load private key file
File private_key = SPIFFS.open("/private.der", "r"); //replace private eith your uploaded file name
if (!private_key) {
Serial.println("Failed to open private cert file");
}
else
Serial.println("Success to open private cert file");

delay(1000);

if (espClient.loadPrivateKey(private_key))
Serial.println("private key loaded");
else
Serial.println("private key not loaded");

// Load CA file
File ca = SPIFFS.open("/ca.der", "r"); //replace ca eith your uploaded file name
if (!ca) {
Serial.println("Failed to open ca ");
}
else
Serial.println("Success to open ca");

delay(1000);

if(espClient.loadCACert(ca))
Serial.println("ca loaded");
else
Serial.println("ca failed");

Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());

   SPI.begin();       // Init SPI bus
   mfrc522.PCD_Init(); // Init MFRC522

   Serial.println("RFID reading UID");

}

void loop() {

if (!client.connected()) {
reconnect();
}
client.loop();

Blynk.run();
digitalWrite(D3, LOW);
String tag= "";
  if ( mfrc522.PICC_IsNewCardPresent())
  {
      if ( mfrc522.PICC_ReadCardSerial())
      {
           
           Serial.print("Tag UID:");
           for (byte i = 0; i < mfrc522.uid.size; i++) {
                  Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                  Serial.print(mfrc522.uid.uidByte[i], HEX);
                  tag.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
                  tag.concat(String(mfrc522.uid.uidByte[i], HEX));
           }
           Serial.println();
           
           Serial.print("Sending UID : ");
           Serial.print(tag);
           Serial.print(" ...");
           if(tag==" 17 01 c2 3b")
           {
              if(! client.publish("outTopic", "Card : 17 01 C2 3B"))
              {
                Serial.println("FAILED!!!");
              }
              else
              {
                Serial.println("Done!");
                digitalWrite(LED_BUILTIN, LOW);
                digitalWrite(D3, HIGH);
                delay(5000);
              }
           }
           
           else if(tag==" 19 44 c6 6e")
           {
              if(! client.publish("outTopic", "Token : 19 44 C6 6E"))
              {
                Serial.println("FAILED!!!");
              }
              else
              {
                Serial.println("Done!");
                digitalWrite(LED_BUILTIN, LOW);
                digitalWrite(D3, HIGH);
                delay(5000);
              }
           }
            
            mfrc522.PICC_HaltA();
     }
  }
//Serial.print("Heap: "); Serial.println(ESP.getFreeHeap()); //Low heap can cause problems

digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
delay(100); // wait for a second
digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
delay(100); // wait for a second
}
