# RFID-enabled-door-lock-system

•	Implemented Internet of Things while designing a door lock, as a part of Home Security System.

•	Components used include ESP8266 WiFi module, RFID RC522 reader, AWS IoT core, Amazon SNS, Blynk App and solenoid lock. The embedded program was written in the Arduino IDE.

•	The RFID reader reads the RFID tag information and sends it to the ESP8266 module which then uploads the information to AWS IoT core cloud platform acting as the MQTT broker. Amazon SNS recieves that information and sends an email notification to the host. The host opens the Blynk App in his/her smartphone to press trigger button. That action is uploaded to the cloud for the ESP module to read. The module receives the trigger message and unlocks the solenoid lock connected to it through a relay.

•	Successfully implemented IoT in a Door Lock System where the guest comes to the door and shows his/her RFID tag. The host, being anywhere in the world, gets an email notification which contains the information of the guest. Then uses the smartphone application to open the door.
