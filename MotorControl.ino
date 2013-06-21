#include <Arduino.h>
#include <LiquidCrystal.h>
#include </home/aurangzaib/Documents/arduino/libraries/custom/Streaming/Streaming.h>
//
#define set digitalRead
#define clr !digitalRead
#define var char
//
#define OEmpty (set(7) && set(8) && set(9) && set(10)) // overhead tank empty condition
#define OFull (clr(7) && clr(8) && clr(9) && clr(10)) // overhead tank full condition
#define OHalf (clr(7) && clr(8) && set(9) && set(10)) // overhead tank half condition
#define OOneThird (clr(7) && clr(8) && clr(9) && set(10)) // overhead tank one-third condition
#define OOneFourth (clr(7) && set(8) && set(9) && set(10)) // overhead tank one-fourth condition
//
#define GEmpty (set(3) && set(4) && set(5) && set(6)) // ground tank empty condition
#define GFull (clr(3) && clr(4) && clr(5) && clr(6)) // ground tank full condition
#define GHalf (clr(3) && clr(4) && set(5) && set(6)) // ground tank half condition
#define GOneThird (clr(3 ) && clr(4) && clr(5) && set(6)) // ground tank one-third condition
#define GOneFourth (clr(3) && set(4) && set(5) && set(6)) // ground tank one-fourth condition
//
#define OError ((!OEmpty) && (!OFull) && (!OHalf) && (!OOneThird) && (!OOneFourth))
#define GError ((!GEmpty) && (!GFull) && (!GHalf) && (!GOneThird) && (!GOneFourth))
#define Error  (( OError) || ( GError ))
//
#define off 0 // wrapper
#define on 1
#define ground 0
#define overhead 1
#define OMotor 13
#define GMotor 12
#define Line 11
#define delaynumber 300
//
void LoadOn(unsigned char);   // starting motors and speakers
void LoadOff(unsigned char);   // stopping motors and speakers
char LineCheck();
void OClearScreen();  // to clear lcd portion for overhead tank
void GClearScreen();  // to clear lcd portion for ground tank
//
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);   // RS E D4 D5 D6 D7
void setup() {
    for(var pin = 3 ; pin <= 11 ; pin += 1) {
        pinMode(pin , INPUT_PULLUP);
    }
    for(int i = 3 ; i <= 11 ; i += 1) {
        digitalWrite(i , HIGH);  // make all sensors high initially
    }
    pinMode(GMotor , OUTPUT);   // GMotor
    pinMode(OMotor , OUTPUT);   // OMotor
    lcd.begin(16 , 2);   // 16x2 lcd
    lcd.clear();
}

void loop() {
    if(OFull) {
        // overhead-tank full condition
        delay(delaynumber);
        if(OFull) {
            // overhead-tank full condition
            OClearScreen(); // row 1 clear
            lcd << "Upper : Full" ;
            delay(delaynumber);
            LoadOff(overhead);
        }
    }
    if(GFull) {
        // ground-tank full condition
        delay(delaynumber);
        if(GFull) {
            // ground-tank full condition
            GClearScreen();
            lcd << "Lower : Full" ;
            delay(delaynumber);
            LoadOff(ground);
        }
    }
    //----------------------------------------------------------------  One-third conditions
    if(OOneThird) {
        // overhead-tank onethird condition
        delay(delaynumber);
        if(OOneThird) {
            // overhead-tank onethird condition
            OClearScreen();
            lcd << "Upper : Medium" ;
            delay(delaynumber);
        }
    }
    if(GOneThird) {
        // ground-tank onethird condition
        delay(delaynumber);
        if(GOneThird) {
            // ground-tank onethird condition
            GClearScreen();
            lcd << "Lower : Medium" ;
            delay(delaynumber);
        }
    }
    //----------------------------------------------------------------  Half conditions
    if(OHalf) {
        // overhead-tank half condition
        delay(delaynumber);
        if(OHalf) {
            // overhead-tank half condition
            OClearScreen();
            lcd << "Upper : Half" ;
            delay(delaynumber);
        }
    }
    if(GHalf) {
        // ground-tank half condition
        delay(delaynumber);
        if(GHalf) {
            // ground-tank half condition
            GClearScreen();
            lcd << "Lower : Half" ;
            delay(delaynumber);
        }
    }
    //----------------------------------------------------------------  One forth conditions
    if(OOneFourth) {
        delay(delaynumber);
        if(OOneFourth) {
            OClearScreen();
            lcd << "Upper : Low" ;
            delay(delaynumber);
        }
    }
    if(GOneFourth) {
        delay(delaynumber);
        if(GOneFourth) {
            GClearScreen();
            lcd << "Lower : Low" ;
            delay(delaynumber);
        }
    }
    //----------------------------------------------------------------  Empty conditions
    if(OEmpty) {
        // overhead-tank empty condition
        delay(delaynumber);
        if(OEmpty) {
            // overhead-tank empty condition
            if(GFull || GOneThird || GHalf || GOneFourth) {
                // checking lower tank for sufficient water
                OClearScreen();
                lcd << "Upper : Very Low" ;
                delay(delaynumber);
                OClearScreen();
                lcd << "Tank filling" ;
                delay(delaynumber);
                LoadOn(overhead);
            } else if(GEmpty) {
                // conditions when not to run upper motor
                delay(delaynumber);
                if(GEmpty) {
                    // conditions when not to run upper motor
                    LoadOff(overhead);
                    OClearScreen();
                    lcd << "Motor can't run";
                    delay(delaynumber);
                    OClearScreen();
                    lcd << "Lower tank low";
                    unsigned char check = LineCheck();
                    if(check == on) {
                        LoadOn(ground);
                    } else if(check == off) {
                        LoadOff(ground);
                        GClearScreen();
                        lcd << "No Line water";
                    }
                }
            }
        }
    }
    if(GEmpty) {
        // ground-tank empty condition
        delay(delaynumber);
        if(GEmpty) {
            // ground-tank empty condition
            GClearScreen();
            lcd << "lower : Very Low";
            delay(delaynumber);
            GClearScreen();
            lcd << "Tank filling";
            delay(delaynumber);
            unsigned char check = LineCheck();
            if(check == on) {
                LoadOn(ground);
            } else if(check == off) {
                LoadOff(ground);
                GClearScreen();
                lcd << "No Line water";
            }
        }
    }
    //-----------------------------------------------------------------  faulty conditions
    if(Error) {
        delay(delaynumber);
        if(Error) {
            lcd.clear();
            lcd << "   SOME ERROR";
            LoadOff(ground);
            LoadOff(overhead);
            while(Error);
        }
    }
}

void OClearScreen() {
    lcd.setCursor(0 , 0);  // selecting just upper portion of lcd
    lcd << "                   "; // gives effect of clear upper lcd row
    lcd.setCursor(0 , 0);
}

///  ____________________________________________________________________________________________ GROUND SCREEN CLEAR
void GClearScreen() {
    lcd.setCursor(0, 1); // selecting just lower portion of lcd
    lcd << "                     "; // gives effect of clear lower lcd row
    lcd.setCursor(0, 1);
}

char LineCheck() {
    LoadOn(ground); // running ground motor for 1 minute
    GClearScreen();
    lcd << "Checking line";
    delay(300);
    if(clr(11)) {
        return on;
    }
    return off; // else if ( LineOff ) { return off ; }
}

void LoadOn(unsigned char number) {
    if(number == overhead) {
        digitalWrite(OMotor , HIGH);
    } else if(number == ground) {
        digitalWrite(GMotor , HIGH);
    }
}

void LoadOff(unsigned char number) {
    if(number == overhead) {
        digitalWrite(OMotor , LOW);
    } else if(number == ground) {
        digitalWrite(GMotor , LOW);
    }
}

