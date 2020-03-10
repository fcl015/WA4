#include <SoftwareSerial.h>

char NOMBRE[10]  = "ESBIN01";    
char BPS         = '4';           // 1=1200 , 2=2400, 3=4800, 4=9600, 5=19200, 6=38400, 7=57600, 8=115200
char PASS[10]    = "1234";        
SoftwareSerial dispositivo(10,11);  // RX=10, TX=11
 
void setup()
{
    dispositivo.begin(9600);
    pinMode(13,OUTPUT);
    digitalWrite(13,HIGH);
    delay(10000);
    digitalWrite(13,LOW);
    
    dispositivo.print("AT"); 
    delay(1000);
 
    dispositivo.print("AT+NAME"); 
    dispositivo.print(NOMBRE);
    delay(1000);
 
    dispositivo.print("AT+BAUD"); 
    dispositivo.print(BPS); 
    delay(1000);
 
    dispositivo.print("AT+PIN");
    dispositivo.print(PASS); 
    delay(1000);    
}
 
void loop()
{
    digitalWrite(13, !digitalRead(13));
    delay(500);
}
