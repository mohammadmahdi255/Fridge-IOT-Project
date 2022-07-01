#include <Wire.h>
#include <ESP8266WiFi.h>
#include "Secrets.h"

#define SLAVE_ADDRESS 8

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;
const char* statusName[] = {"Emergency", "Fetal Error", "Off", "Init Up", "Cooling", "Door is Open", "Sleep", "Not Found"};

WiFiServer server(80);

String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void sendData(int address, String data) {
    Serial.println(data);
    Wire.beginTransmission(address);
    for (unsigned int i = 0; i < data.length(); i++) {
        Wire.write(data[i]);
    }
    Wire.endTransmission();
}

void recvData(int address, byte data[], int size) {
    Wire.requestFrom(address, size);
    for (int i = 0; Wire.available(); i++)
        data[i] = Wire.read();
    delay(70);
}

const char* getStatus(long currentStatus) {
    if(currentStatus < -2 || currentStatus > 4)
        return statusName[7];

    return statusName[currentStatus + 2];
}