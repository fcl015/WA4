#include <SoftwareSerial.h>

int led=13;
int estado=0;
SoftwareSerial dispositivo(10,11);  // RX=10, TX=11

void setup(){
  pinMode(led,OUTPUT);
  dispositivo.begin(9600);
}

void loop(){
  if(dispositivo.available()>0){
    estado=dispositivo.read();
  }
  
  if(estado=='A'){
    digitalWrite(led,HIGH);
  }
  if(estado=='B'){
    digitalWrite(led,LOW);
  }
}
