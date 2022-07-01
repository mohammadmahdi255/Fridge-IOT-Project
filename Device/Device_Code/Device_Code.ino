#include "Device_Code.h"

ControlUnit centerUnit(PIN_COMPRESSOR, PIN_LAMP, PIN_FAN, PIN_IONIZER, PIN_BUZZER, true, KEY_READ, PIN_LM35, PIN_DOOR, sev, PIN_SEV0, PIN_SEV1);

void setup() {
  // put your setup code here, to run once:
    Wire.begin(SLAVE_ADDRESS);
    Wire.onReceive(receiveEvent); /* register receive event */
    Wire.onRequest(requestEvent); /* register request event */
    Serial.begin(115200);           /* start serial for debug */
}

void loop() {
  // put your main code here, to run repeatedly:
  centerUnit.process();
}

void receiveEvent(int howMany) {
    Serial.println("recv (byte): " + String(howMany)); 
    Serial.print("recv : "); 
    String message = "";
    while (0 <Wire.available()) {
        message += (char)Wire.read();      /* receive byte as a character */
    }
    
    Serial.println(message);
    if(howMany == 1) {
        centerUnit.readWebKey = message.toInt();
    } 
}

// function that executes whenever data is requested from master
void requestEvent() {
    byte* px[3];
    float currentTemp = centerUnit.getCurrentTemperature();
    float requestTemp = centerUnit.getTemperature();
    long currentState = centerUnit.getState();
    Serial.println(currentState);
    px[0] = (byte*)&currentTemp;
    px[1] = (byte*)&requestTemp;
    px[2] = (byte*)&currentState;
    for(int i = 0; i < sizeof(float) * 2 + sizeof(long); i++) {
        Serial.print((byte)px[i]);
        Serial.print(" ");
    }
    Serial.println();
    Wire.write(px[0], sizeof(float));
    Wire.write(px[1], sizeof(float));
    Wire.write(px[2], sizeof(long));
}