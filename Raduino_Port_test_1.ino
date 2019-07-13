/*
 * Testprogramm für Raduino de DL1YAR Mai 2019
 * Basis ist der Raduino zum BITX40
 * angepasst an den vorhandenen TRX
 * mit diesem Sketch sollen der eigene Aufbau
 * getested werden und evtl Änderungen werden 
 * in die  
 * 
 * Raduino_PortPinUser.h
 * 
 * eingetragen. Diese Datei wird im Raduinosketch 
 * wieder entsprechend gebraucht und MUSS dahin
 * kopiert werden
 * Es wird eine kleine LED-Schaltung als Tester
 * benötigt!
 */
#include "Raduino_PortPinUser.h"


#define NOP __asm__ __volatile__ ("nop\n\t")


//***********************************************************************
const int testpin = dreh_a; //  Hier Testpin eintragen ******************

//***********************************************************************

#define my_ver "RaduinoPortTest DL1YAR Juli2019" 

//******************************************
void testport(){
  pinMode(testpin,OUTPUT);
  digitalWrite(testpin, !digitalRead(testpin));
  Serial.println(testpin);
  delay(222);             // anpassen Blinkfrequenz *******
}

//******************************************
//******************************************
void setup() {
  // put your setup code here, to run once:
    Serial.begin(baud);
    Serial.println(my_ver);
    Serial.println(mycall);
     delay(2000);

}

//******************************************
void loop() {
  // zum Testen
  testport();
  Serial.print("Tastenwert: ");
  Serial.println(analogRead(mode_taste));
  
}
