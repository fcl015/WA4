
/*==============================================================================================*/
/*=====                      Change BT Name                                                =====*/
/*=====                       ARDUINO MKR                                                  =====*/
/*=====                    Water Automation                                                =====*/
/*=====                                                                                    =====*/
/*==============================================================================================*/
#include <math.h> 
#include <Arduino.h>        // required before wiring_private.h
#include "wiring_private.h" // pinPeripheral() function

#define LED_SIGNAL          A3  // A3 as an indicator led
#define TURN_ON_LED()       digitalWrite(LED_SIGNAL,HIGH)
#define TURN_OFF_LED()      digitalWrite(LED_SIGNAL,LOW)

char NOMBRE[10]  = "VALVULAS";    
char BPS         = '4';           // 1=1200 , 2=2400, 3=4800, 4=9600, 5=19200, 6=38400, 7=57600, 8=115200
char PASS[10]    = "1234";        

//---------------------------------------------------------
// Set-up function
//---------------------------------------------------------
void setup(){
  Serial.begin(9600);   // Configure USB serial as serial monitor

  Serial1.begin(9600);  // Configure Pins 13(RX),14(TX) for Bluetooth communication 
  while(!Serial1);

  pinMode(LED_SIGNAL, OUTPUT);
  delay(10000);

    TURN_ON_LED();
    Serial1.print("AT"); 
    delay(1000);
 
    Serial1.print("AT+NAME"); 
    Serial1.print(NOMBRE);
    delay(1000);
 
    Serial1.print("AT+BAUD"); 
    Serial1.print(BPS); 
    delay(1000);
 
    Serial1.print("AT+PIN");
    Serial1.print(PASS); 
    delay(1000); 
    TURN_OFF_LED();   

  
}

//---------------------------------------------------------
// Main loop
//---------------------------------------------------------
void loop(){ 

    TURN_OFF_LED();
    delay(500);
    TURN_ON_LED();
    delay(500);
}


