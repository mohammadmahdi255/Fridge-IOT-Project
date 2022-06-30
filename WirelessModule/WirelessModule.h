#include <Wire.h>
#include <ESP8266WiFi.h>
#include "Secrets.h"

#define SLAVE_ADDRESS 8

const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;

WiFiServer server(80);

String header;

// Auxiliar variables to store the current output state
String output5State = "off";
String output4State = "off";

// Assign output variables to GPIO pins
const int output5 = 2;
const int output4 = 16;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void sendData(int address, String data) {
      Serial.println(data);
      Wire.beginTransmission(address);
      for(unsigned int i=0; i < data.length(); i++) {
            Wire.write(data[i]);
      }
      Wire.endTransmission();
}

void recvData(int address, byte data[], int size) {
      Wire.requestFrom(address, size); 
      for(int i = 0; Wire.available(); i++){
            data[i] = Wire.read();
            Serial.print((int)data[i]);
            Serial.print("-");
      }
      Serial.println();
      delay(70);
}

String getStatus(int currentStatus) {
      switch(currentStatus) {
            case -2:
                  return "Emergency";
            case -1:
                  return "Fetal Error";
            case 0:
                  return "Off";
            case 1:
                  return "Init Up";
            case 2:
                  return "Cooling";
            case 3:
                  return "Door is Open";
            case 4:
                  return "Sleep";
            default:
                  return "Not found";
      }
}
