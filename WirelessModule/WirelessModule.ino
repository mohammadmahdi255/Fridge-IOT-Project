#include "WirelessModule.h"


void setup() {
    Serial.begin(115200);
    Wire.begin(D1, D2);

    // Initialize the output variables as outputs
    pinMode(output5, OUTPUT);
    pinMode(output4, OUTPUT);
    // Set outputs to LOW
    digitalWrite(output5, LOW);
    digitalWrite(output4, LOW);

    // Connect to Wi-Fi network with SSID and password
    sendData(SLAVE_ADDRESS, "Connecting to ");
    sendData(SLAVE_ADDRESS, String(ssid));
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    // Print local IP address and start web server
    sendData(SLAVE_ADDRESS, "");
    sendData(SLAVE_ADDRESS, "WiFi connected.");
    sendData(SLAVE_ADDRESS, "IP address: ");
    sendData(SLAVE_ADDRESS, WiFi.localIP().toString());
    server.begin();
}

void loop() {
    WiFiClient client = server.available();  // Listen for incoming clients
    delay(2000);

    if (client) {

        Serial.println("New Client.");
        String currentLine = "";
        previousTime = millis();
        
        while (client.connected() && millis() - previousTime <= timeoutTime) {

            if (client.available()) {
                char c = client.read();
                Serial.write(c);
                header += c;

                if (c == '\n' && currentLine.length() == 0) {
                    if (header.indexOf("GET /1/") >= 0) {
                        sendData(SLAVE_ADDRESS, "2");  // inc temp is 2
                        client.println("'{\"message\": \"successfull\"}'");
                    } else if (header.indexOf("GET /2/") >= 0) {
                        sendData(SLAVE_ADDRESS, "3");  // dec temp is 3
                        client.println("'{\"message\": \"successfull\"}'");
                    } else if (header.indexOf("GET /3/") >= 0) {
                        sendData(SLAVE_ADDRESS, "1");  // mute is 1
                        client.println("'{\"message\": \"successfull\"}'");
                    } else if (header.indexOf("GET /4/") >= 0) {
                        sendData(SLAVE_ADDRESS, "4");  // super cooling is 4
                        client.println("'{\"message\": \"successfull\"}'");
                    } else if (header.indexOf("GET /5/") >= 0) {
                        sendData(SLAVE_ADDRESS, "0");  // power on/off is 0
                        client.println("'{\"message\": \"successfull\"}'");
                    } else if (header.indexOf("GET /info/") >= 0) {
                        byte data[12];
                        char buff[512];
                        recvData(SLAVE_ADDRESS, &data[0], 12);
                        float currentTemp = *((float *)&data[0]);
                        float requstTemp = *((float *)&data[4]);
                        int currentState = *((int *)&data[8]);
                        sprintf(buff, "'{%s:\"%f\",%s:\"%f\",%s:\"%s\"}'", "current_temperature", currentTemp, "request_temp", requstTemp, "current_status", getStatus(currentState));
                        Serial.println(buff);
                        client.println(buff);
                    }
                    break;
                } else if(c == '\n') {
                    currentLine = "";
                } else if (c != '\r') {
                    currentLine += c;
                }
            }
        }

        header = "";
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }
}