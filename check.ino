// i am going to make a fucking simple server on arduino
//
#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include <Streaming.h>

byte mac[ ] = {0X00, 0XAA, 0XBB, 0XCC, 0XDE, 0X01} ;
IPAddress deadSimpleServerip(192, 168, 0, 100) ;
EthernetServer server(80) ;
void setup() {
    Serial.begin(9600) ;
    delay(666) ;
    Ethernet.begin(mac, deadSimpleServerip) ;
    server.begin() ;
    Serial << "sucking serial working" << endl ;
}

void loop() {
    EthernetClient client = server.available() ;
    if (client) {
        Serial << "dickless client connected" << endl ;
        while (client.connected()) {
            if (client.available()) {
                char suckingClientData = client.read() ;
                Serial << suckingClientData << " " ;
                client << "asshole you sent shit to the server" << endl ;
            }
        }
        client.stop() ;
    }
}
