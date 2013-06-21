#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include <Streaming.h>

byte mac[ ] = { 0X00, 0XAA, 0XBB, 0XCC, 0XDE, 0X01};
IPAddress ip(192, 168, 0, 102); // arduino ip a/d
IPAddress server(192, 168, 0, 101); // localhost a/d, 127.0.0.1 is loopback a/d
EthernetClient client; // arduino as client
boolean requested = false;
int led = 8 ;
void connectServer() {
    if (!client.connected()) {
        if (client.connect(server, 80)) {
            Serial << "connected"<< endl;
            requested = false;
        }
        else {
            Serial << "cant connect"<< endl;
        }
    }
}

boolean makeRequest() {
    client << "GET /myscript.php HTTP/1.1\n"; // file in local server
    client << "HOST:192.168.0.101\n\n"; // server a/d
    // saying i need to request myscript.php file on server 192.168.0.101
    client << endl; // sending linefeed
    return true; // requested = true
}

void setup() {
    Ethernet.begin(mac, ip);
    Serial.begin(9600);
    delay(2000);
    connectServer();
    pinMode(led, OUTPUT) ;
}

void loop() {
    if (client.connected()) {
        if (!requested) {
            // i.e. client is connected but req. hasnt been sent
            Serial << "sending request"<< endl;
            requested = makeRequest(); // requesting server
        }
        else {
            // i.e. client is connected and req. has been sent
            delay(15);
            if (client.find("Your result: ")) {
                int airReadings = client.parseInt();
                Serial << "Your result: "<< airReadings << endl; // sending to serial port, in other words to processing
                if (airReadings == 1023) { // go urself to site and see value then use led for confirmation
                    digitalWrite(led, HIGH) ;
                }
                else {
                    digitalWrite(led ,LOW);
                }
            }
        }
    }
    else {
        client.stop();
        connectServer();
    }
}

