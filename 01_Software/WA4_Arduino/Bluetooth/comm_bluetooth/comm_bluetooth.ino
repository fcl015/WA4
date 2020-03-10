#include <SoftwareSerial.h> //COM4

char value1;
char value2;

SoftwareSerial dispositivo(10,11); //Rx=pin10 Tx=pin11

void setup() { //master bt
  dispositivo.begin(115200);
  delay(100);
  Serial.begin(9600);
  delay(100);
  }


void loop() {


  if(dispositivo.available()>0){
    value1=dispositivo.read();
    Serial.print(value1);
  }

  if(Serial.available()>0) {
    value2=Serial.read();   
    //Serial.print(value2);
    dispositivo.write(value2);
  }


}
