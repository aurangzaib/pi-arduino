#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Xively.h>

// Your Xively key to let you upload data
char xivelyKey[] = "c7EWXahTwHLwsk9LO09mKGgTc7mBznvIMhM6Kyw0zzGeGywk";
//your xively feed ID
#define xivelyFeed 1379424460
//datastreams
char sensorID[] = "photocell";
char ledID[] = "led";
// Analog pin which we're monitoring (0 and 1 are used by the Ethernet shield)
#define sensorPin A2
//led connected pin
#define ledPin 9

IPAddress ip(192, 168 , 0 , 102) ;

byte mac [ ] = { 0X00, 0XAA, 0XBB, 0XCC, 0XDE, 0X01};

// Define the strings for our datastream IDs
XivelyDatastream datastreams[] = {
    XivelyDatastream(sensorID, strlen(sensorID), DATASTREAM_FLOAT),
    XivelyDatastream(ledID, strlen(ledID), DATASTREAM_FLOAT),
} ;
// Finally, wrap the datastreams into a feed
XivelyFeed feed(xivelyFeed, datastreams, 2);
EthernetClient client;
XivelyClient xivelyclient(client);

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    //pin setup
    pinMode(sensorPin, INPUT);
    pinMode(ledPin, OUTPUT);
    Serial.println("Starting single datastream upload to Xively...");
    Serial.println();
    // attempt to connect to Wifi network:
    Ethernet.begin(mac, ip) ;
    delay(1000) ;
    Serial.println("Connected to Ethernet");
}

void loop() {
    //adjust LED level. set from Xively
    int getReturn = xivelyclient.get(feed, xivelyKey); //get data from xively
    if (getReturn > 0) {
        Serial.println("LED Datastream");
        Serial.println(feed[1]);
    }
    else { Serial.println("HTTP Error"); }
    //write value to LED - change brightness
    int level = feed[1].getFloat();
    if (level < 0) {
        level = 0;
    }
    else if (level > 255) {
        level = 255;
    }
    //actually write the value
    digitalWrite(ledPin, level);
    ///////////////////////////////////////////////////////
    //read sensor values
    int sensorValue = analogRead(sensorPin);
    datastreams[0].setFloat(sensorValue);
    //print the sensor valye
    Serial.print("Read sensor value ");
    Serial.println(datastreams[0].getFloat());
    //send value to xively
    Serial.println("Uploading it to Xively");
    int ret = xivelyclient.put(feed, xivelyKey);
    //return message
    Serial.print("xivelyclient.put returned ");
    Serial.println(ret);
    Serial.println("");
    //delay between calls
    delay(15000);
}

