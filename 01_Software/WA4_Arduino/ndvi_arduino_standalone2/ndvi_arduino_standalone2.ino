#include<stdlib.h>

int s2=6;

unsigned long tiempo=10000;
unsigned long tiempoanterior=0;

float fixed_alpha=1.86;
float alpha_data=1.86;
float red_data=0.0;
float nir_data=0.0;
float ndvi_data=0.0;

char message[23];
char sensor_type=0;
char sensor_orientation=0;
char ndvi_error_code=0;

/*-------------Metodo para leer NIR and RED reflectance ------------*/
void readNVDI(){

    char dato1[7], dato2[7];
    int i;

    //Energizar sensor
    Serial.println("ACTIVATE READING"); 
    digitalWrite(s2, HIGH);
    delay(1000);
    
    //Empty message buffer
    for(int i=0; i<23; i++){
          message[i]=0;     
    }
    //Esperar y leer datos provenientes del sensor
    if (Serial1.available()) {   

        Serial1.readBytes(message,23);
        Serial.println("----------------------------"); 
        Serial.print("RAW MESSAGE  :"); 
        Serial.print(message);
        Serial.print("MESSAGE SIZE :");
        Serial.println(23,DEC);
        
        Serial.print("MESSAGE HEX  :");
        for(int i=0; i<=23; i++){
          Serial.print(message[i],HEX);      
          Serial.print(' '); 
        }
        Serial.println(' '); 

        // Extract red and nir and ndvi from string
        int state=0;
        int j=0;
        for(int i=0; i<23; i++){
              switch(state){
                case 0:   if(message[i]==0x09){   // 0x09 Tab character signaling the start of the data transmission
                            state++;
                          }
                          break;
                case 1:   if(message[i]==' '){   // Space character separate red data from nir data
                            state++;
                            dato1[j]=0;
                            j=0;
                          }
                          else{                 // Capture red data until a space character is found
                            dato1[j]=message[i];
                            j++;
                          }
                          break;
                case 2:   if(message[i]==' '){   // Space character separate nir data from sensor orientation
                            state++;
                            dato2[j]=0;
                            j=0;
                          }
                          else{                 // Capture nir data until a space character is found
                            dato2[j]=message[i];
                            j++;
                          }
                          break;
                case 3:   sensor_orientation=message[i];  // Capture sensor orientation byte
                          sensor_type=message[i+2];       // Capture sensor type
                          state++;
                          break;
                defult:   break;
              }
        }
        //Convertir arreglo "a" a variable flotante
        red_data = atof(dato1); 
        //Convertir arreglo "b" a flotante 
        nir_data = atof(dato2);
        //Calcular NVDI si el sensor tiene orientacion hacia abajo '1' , tipo de sensor de hemisferico 'r'
        if( sensor_type=='r' and sensor_orientation=='1' ){
              ndvi_data = (alpha_data*nir_data - red_data) / (alpha_data*nir_data + red_data);
              ndvi_error_code=0;
        }
        //Si no, error en la lectura de NDVI 
        else {
              red_data = -1.0;
              nir_data = -1.0;
              ndvi_data = -1.0;
              ndvi_error_code=1;
        }
    }
    else{
        red_data = -1.0;
        nir_data = -1.0;
        ndvi_data = -1.0;
        ndvi_error_code=2;
    }

    Serial.print("RED: ");
    Serial.println(red_data,4);
    Serial.print("NIR: ");
    Serial.println(nir_data,4);
    Serial.print("NDVI: ");
    Serial.println(ndvi_data,4);
    
    //Empty serial input buffer
    while(Serial1.available()){
            Serial1.read();
    }
    
    //Desenergizar sensor
    digitalWrite(s2, LOW);
    delay(100);
    Serial.println("TERMINATE READING"); 

}



void setup() {
   pinMode(s2, OUTPUT);
   Serial.begin(9600);//Puerto para comunicacion por xbee
   Serial1.begin(1200);//Puerto de comunicacion con sensor digital
}
 
void loop() {
  
    unsigned long tiempoactual=millis();

    if(tiempoactual - tiempoanterior > tiempo) {
          tiempoanterior = tiempoactual;
          readNVDI();
    }
}

