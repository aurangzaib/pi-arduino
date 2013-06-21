// libraries

#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include <SD.h>
#include <TextFinder.h>
#include <Streaming.h>
#include <EEPROM.h>

// globals

long now=0 ; // holding time

const int thermostatAddress = 10 , // eeprom a/d
          sensorPin = A0,
          relayPin = A1 ,
          sdChipSelect = 4 , // ss for sdcard
          timeFrame = 10000 , // min. time for sensing
          requestLength=6, // length of request
          fileLength=16 ; // length of filename

int thermostat = EEPROM.read(thermostatAddress); // set temp

char request[requestLength] , // request i.e. GET or POST
     file[fileLength] ; // file requested by client

byte mac[ ] = {0X00, 0XAA, 0XBB, 0XCC, 0XDE, 0X01} ; // hardware a/d
IPAddress ip(192, 168, 0,100) ; // ip a/d of arduino server
EthernetServer server(80) ; // http server

// library declaration

float sensing() ;
void relayFunction() ;
void sendFile(EthernetClient client, char file[ ]) ;
void sendHttpHeader(EthernetClient client, int code) ;

// setup

void setup() {
    //
    // start serial , ethernet , server, and sd card
    // set ddr of relay and sensor
    //
    Serial.begin(9600) ;
    delay(700) ;
    Serial << "Serial port available now" << endl ;
    Ethernet.begin(mac, ip) ;
    server.begin() ;
    if (!SD.begin(sdChipSelect)) {
        Serial << "sd card not functional" << endl ;
    }
    else {
        Serial << "sd card functional" << endl ;
    }
    pinMode(sensorPin, INPUT) ;
    pinMode(relayPin, OUTPUT) ;
}

// main loop

void loop() {
    String fileName = " " ;
    int fileNameLength = 0 ,
        requestState=0; // 1=>GET , 2=>POST
    EthernetClient client = server.available() ;
    if (client) { // if client accessible
        TextFinder finder(client) ; // TextFinder instance
        while (client.connected()) {
            if (client.available()) {
                //
                // getString format => from , till , save found string , length
                //
                if (finder.getString("", "/",request, requestLength)) {
                    // GET or POST request
                    if (String(request) == "GET") {
                        requestState = 1 ;
                    }
                    else if (String(request) == "POST") {
                        requestState = 2 ;
                    }
                    fileNameLength =  finder.getString(""," ",file,fileLength) ;
                    // filename client requested
                    // getString result will be saved in
                    // file[ fileLength ] while length
                    // will be saved in fileNameLength
                    // fileNameLength has actual length
                    // fileLength has max array size
                    //
                    switch (requestState) {
                    case 1 : // GET
                        break ; // just send index.html below
                    case 2 : // POST
                        finder.find("\n\r") ; // find until \n\r
                        if (finder.find("thermostat")) { // if thermostat found
                            int newThermostat =  finder.getValue('=') ;
                            // get value written after "="
                            if (newThermostat != thermostat) {
                                // if new value found
                                thermostat = newThermostat ;
                                thermostat = constrain(thermostat, 20, 40) ;
                                // setting range of values
                                EEPROM.write(thermostatAddress, thermostat) ;
                                // also write to EEPROM
                            }
                        }
                        break ;
                    }
                    if (fileNameLength < 2) {
                        sendFile(client , "index.htm") ;
                        // "/" means root i.e. index.htm
                    }
                    else {
                        sendFile(client , file) ;
                        // send whatever filename client asked
                    }
                }
                delay(5) ;
                Serial << "Closing connection" << endl ;
                client.stop() ; // close client session and socket
            }
        }
    }
    if ((millis()-now) > (timeFrame)) {  // if certain time passed
        boolean relayState = relayFunction() ;
        if (relayState == true) {
            digitalWrite(relayPin, HIGH) ;
        }
        if (relayState == false) {
            digitalWrite(relayPin, LOW) ;
        }
    }
    now = millis() ;
}

// library definition

float sensing() {
    //
    // sense value in binary form , change it to
    // voltage then in temp form
    //
    int binaryForm = analogRead(A0) ; // raw form
    float voltageForm = (binaryForm * 5) / 1024 ;
    float temperatureForm = (voltageForm - 0.5) * 100 ;
    return temperatureForm ;
}

boolean relayFunction() {
    //
    // use sensing( ) func, and
    // send true if current value > thermostat
    //
    if (sensing() > thermostat) {
        return true ;
    }
    return false ;
}

void sendFile(EthernetClient client, char file[ ]) {
    //
    // open file , check its accessiblity, read per char and save in string
    // whenever any html var is found in string, replace it with
    // respective arduino var, this way you can change html page with arduino
    // value, then close the file
    //
    String readingFile = " " ; // to save html file in string form
    File openFile = SD.open(file) ; // opening file
    sendHttpHeader("HTTP/1.1 ") ; // sending general http header
    if (openFile) { // if file is accessible
        sendHttpHeader(client, 200) ; // send OK header
        while (openFile.available()) { // till reading file
            char reading = openFile.read() ; // save per char
            readingFile += reading ; // cascade in string
            if (readingFile.endsWith("$temperature")) {
                // if $temperature is found in html, change it with arduino var
                // since arduino has no float to string func
                // so  we'll send that arduino directly to client
                readingFile = " " ;
                client << (sensing() , 2) ; // 2 decimals
            }
            if (readingFile.endsWith("$thermostat")) {
                // if arduino var value is int, then you can use replace
                // func i.e. replace htmlVar with String(arduinoVar)
                readingFile.replace("$thermostat" , String(thermostat)) ;
            }
            if (readingFile.endsWith("$status")) {
                String relayState = "off" ; // making string form
                if (relayFunction()) {
                    relayState = "on" ;
                }
                readingFile.replace("$status", relayState) ;
            }
            if (reading == '\n') {
                // when file has \n, send all string
                client << readingFile ;
                readingFile = "" ;
            }
        }
        openFile.close() ; // close the file
    }
    else {
        sendHttpHeader(client, 404) ;
        // if file not accessible, send not found
    }
}

void sendHttpHeader(EthernetClient client, int code) {
    //
    // send http general header then response header a/cingly
    //
    client << "HTTP/1.1 " ; // standard header sent to browser
    if (code == 200) {
        client << "200 OK" ;
        client << "Content-Type: text/html";
    }
    else if (code == 404) {
        client << "404 Not Found" ;
    }
    client << endl ; // terminating with linefeed
}
