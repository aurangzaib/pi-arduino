/*

in this program , arduino behaves as a server i.e. it sends info to browser

*/

/*
// when you are server then to print serially whats coming from client ( i.e. browser ) :

EthernetServer server( 80  ); // http server
EthernetClient client = server.available( ) ;
if ( client.connected( ) ) {
    if ( client.available( ) ){
    char someChar = client.read( );
    Serial << someChar ;
    }
}

// when you are client then to connect to a server :

IPAddress server (  ip of the server you wanna connect, either its local or something )
EthernetClient client ;
if ( client.connect( server , 80 ) ) {
    Serial << "connected to server " ;
} else {
    Serial << "connection to server failed " ;
}
*/


#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include <Streaming.h>

EthernetServer server(80); // http
int analogPins[ ] = { A0, A1, A2, A3, A4, A5} ;
byte mac[ ] = { 0X00, 0XAA, 0XBB, 0XCC, 0XDE, 0X01} ;
IPAddress ip(192, 168, 0, 103) ;

void makeResponse(EthernetClient thisClient)  ;
float sensing() ;
int counter = 0 ;
//
// setup function
//
void setup() {
    pinMode(A0, INPUT) ;
    Ethernet.begin(mac, ip) ;
    Serial.begin(9600) ;
    server.begin() ;
    delay(2000);
    Serial << ("Connecting now..") ;
}
//
// main function
//
void loop() {
    EthernetClient client = server.available() ; // server is arduino
    if (client) {
        Serial << (" got a client ") ;
        int lineLength = 0 ;
        while (client.connected()) {
            if (client.available()) {
                char someChar = client.read() ;
                Serial << someChar ;
                makeResponse(client) ; // client is browser
            }
            delay(30) ;
        }
        client.stop() ;
        Serial << "\n\n" << "client is disconnected" << endl ;
    }
}
void makeResponse(EthernetClient thisClient) {
    delay( 20 ) ;
    if (counter == 0) {
        thisClient << "Value at A0 : "  ;
    }
    thisClient << " is : "
               << sensing()
               << ", " ;
    counter+= 1 ;
    if (counter == 7) {
        thisClient << endl ;
    }
}

float sensing() {
    int binaryForm = analogRead(A0) ;
    float voltageForm= float((binaryForm * 5.0)/1024.0) ;
    return voltageForm ;
}
