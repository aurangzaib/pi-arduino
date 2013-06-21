#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include <SD.h>
#include <TextFinder.h>
#include <LCD.h>
#include <Streaming.h>

int sensor = A0 ;
void setup() {
    Serial.begin(9600) ;
    pinMode(sensor , INPUT) ;
    delay(500) ;
    Serial << " Serial port working" << endl ;
}

void loop() {
    int binaryForm = analogRead(sensor) ;
    float voltageForm= float((binaryForm * 5.0)/1024.0) ;
    Serial << voltageForm << endl ;
    delay(35) ;
}
