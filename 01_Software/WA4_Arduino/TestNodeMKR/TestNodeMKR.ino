
/*===================================================================================================================================*/
/*=====                                          Sensor node firmware v4.0                                                      =====*/
/*=====                                                   ARDUINO MKR                                                           =====*/
/*=====                                              Water Automation                                                           =====*/
/*=====                                                                                                                         =====*/
/*=====     To power sensors use digital pins 4, 5 and 6                                                                        =====*/
/*=====     To read sensors use analog pins A4, A5, A6                                                                          =====*/
/*=====                                                                                                                         =====*/
/*=====                                                                                                                         =====*/
/*===================================================================================================================================*/
#include <math.h> 
#include <Arduino.h>        // required before wiring_private.h
#include "wiring_private.h" // pinPeripheral() function

/*=====Timer variables=====*/
unsigned long previousMillis;
unsigned long timeInterval = 10000;
/*=========================*/

/*=====Arduino Input/Outputs definitions=====*/
int S1_ON = 4, S2_ON = 5, S3_ON = 6;
int SDI1_ON = 7, SDI2_ON =10;
int S1_PORT = A4, S2_PORT = A5, S3_PORT = A6;
int PULSE_IN = A1; 

/*======Soil Moisture Variables========*/
int S1_RAW, S2_RAW, S3_RAW;
float S1_MOIST, S2_MOIST, S3_MOIST;
int S1_MOIST_INT, S1_MOIST_DEC, S2_MOIST_INT, S2_MOIST_DEC, S3_MOIST_INT, S3_MOIST_DEC;
char soil_moisture_equation='1';

/*======NDVI Variables========*/
float fixed_alpha=1.86;
float alpha_data=0.0;
float red_data=0.0;
float nir_data=0.0;
float ndvi_data=0.0;
char sensor_type=0;
char sensor_orientation=0;
char NDVI_INT, NDVI_DEC;

/*======Davis Cup Variables========*/
volatile unsigned int totalpulsos=0; 
volatile unsigned int ContactBounceTime=0;
float totalpulsosrps;
float velocidad;
unsigned int velocidad_ent;
unsigned int velocidad_dec;
float velocidad_ant = 0;

/*=====Communication variables=====*/
char input_message_size=22;
char output_message_size=32;
char Buffout[80], Buffin[80];
char XBeeAdd[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char NODE_TYPE = 'S';
int sum1;
/*=================================*/

//---------------------------------------------------------
// Create the new UART instance for Xbee communication assigning it to pin 8(TX) and 9(RX)
//---------------------------------------------------------
Uart SerialXbee(&sercom1, 8, 9, SERCOM_RX_PAD_1, UART_TX_PAD_0);
//---------------------------------------------------------
// Create the new UART instance for SDI_1 assigning it to pin 0(TX) and 1(RX)
//---------------------------------------------------------
Uart SerialSDI_1(&sercom3, 0, 1, SERCOM_RX_PAD_1, UART_TX_PAD_0);
//---------------------------------------------------------
// Create the new UART instance for SDI_2 assigning it to pin 2(TX) and 3(RX)
//---------------------------------------------------------
Uart SerialSDI_2(&sercom0, 2, 3, SERCOM_RX_PAD_3, UART_TX_PAD_2);


//---------------------------------------------------------
// Set-up function
//---------------------------------------------------------
void setup(){
  Serial.begin(9600);   // Configure USB serial as serial monitor

  Serial1.begin(9600);  // Configure Pins 13(RX),14(TX) for Bluetooth communication 
  while(!Serial1);
  
  SerialXbee.begin(9600); // Configure Pins 8(TX),9(RX) for Xbee communication
  while(!SerialXbee);
  pinPeripheral(8, PIO_SERCOM);  
  pinPeripheral(9, PIO_SERCOM);  
  
  SerialSDI_1.begin(1200); // Configure Pins 0(TX),1(RX) for SDI_1 communication
  while(!SerialSDI_1);
  pinPeripheral(0, PIO_SERCOM);  
  pinPeripheral(1, PIO_SERCOM);  
  
  SerialSDI_2.begin(1200); // Configure Pins 2(TX),3(RX) for SDI_2 communication
  while(!SerialSDI_2);
  pinPeripheral(2, PIO_SERCOM);  
  pinPeripheral(3, PIO_SERCOM);  

  pinMode(S1_ON, OUTPUT);
  pinMode(S2_ON, OUTPUT);
  pinMode(S3_ON, OUTPUT);
  pinMode(SDI1_ON, OUTPUT);
  pinMode(SDI2_ON, OUTPUT);

  pinMode(A1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(A1), readWind, FALLING); // Interrupcion en pin A1/16 or A2/17 para conteo por hardware
  
  analogReadResolution(12);
  analogReference(AR_INTERNAL1V0);
}

//---------------------------------------------------------
// Main loop
//---------------------------------------------------------
void loop(){ 
   unsigned long currentMillis = millis();
 
    // IF TimeInterval is over, then calculate soil moisture and NDVI
    if(currentMillis - previousMillis > timeInterval) {
 
    // Obtain Soil Moisture
        previousMillis = currentMillis;   
        S1_RAW = SensorRead(S1_ON, S1_PORT, 100);
        S2_RAW = SensorRead(S2_ON, S2_PORT, 100);
        S3_RAW = SensorRead(S3_ON, S3_PORT, 100);
    
        S1_MOIST = MoistureCalculation(S1_RAW, soil_moisture_equation, 12, 1.0);
        S2_MOIST = MoistureCalculation(S2_RAW, soil_moisture_equation, 12, 1.0);
        S3_MOIST = MoistureCalculation(S3_RAW, soil_moisture_equation, 12, 1.0);
    
        S1_MOIST_INT = int(S1_MOIST);
        S1_MOIST_DEC = (float(S1_MOIST - S1_MOIST_INT))*100;
        S2_MOIST_INT = int(S2_MOIST);
        S2_MOIST_DEC = (float(S2_MOIST - S2_MOIST_INT))*100;
        S3_MOIST_INT = int(S3_MOIST);
        S3_MOIST_DEC = (float(S3_MOIST - S3_MOIST_INT))*100;

        Serial.println("--------------------------");
        Serial.print("S1=");
        Serial.println(S1_MOIST);
        Serial.print("S2=");
        Serial.println(S2_MOIST);
        Serial.print("S3=");
        Serial.println(S3_MOIST);

    // Obtain NDVI
        readNDVI();
        Serial.print("RED=");
        Serial.println(red_data,4);
        Serial.print("NIR=");
        Serial.println(nir_data,4);              
        //Calcular NVDI si el sensor tiene orientacion hacia abajo '1' , tipo de sensor de campo 'r'
        if( sensor_type=='r' and sensor_orientation=='1' ){
              ndvi_data = (fixed_alpha*nir_data - red_data) / (fixed_alpha*nir_data + red_data);
              Serial.print("FIXED ALPHA=");
              Serial.println(fixed_alpha,4);
              Serial.print("NDVI=");
              Serial.println(ndvi_data,4);
        }
        //Calcular factor alfa si el sensor tiene orientacion hacia arriba '2', tipo de sensor de campo 'r'
        else if( sensor_type=='r' and sensor_orientation=='2' ){
              alpha_data = red_data / nir_data;
              ndvi_data = 0.0;
              Serial.print("ESTIMATED ALPHA=");
              Serial.println(alpha_data,4);
              Serial.println("NDVI: NO DATA");
        }
        else{
              Serial.println("NDVI INVALID MEASUREMENT !!!");
        }
        NDVI_INT = int(ndvi_data);
        NDVI_DEC = (float(ndvi_data - NDVI_INT))*100;

    // Obtain Wind Velocity
        totalpulsosrps = float(totalpulsos)/(timeInterval/1000);
        velocidad = 2.25*0.44704*totalpulsosrps; 
        velocidad_ent = velocidad;
        velocidad_dec = (velocidad - velocidad_ent)*100;
        Serial.print("WIND PULSES=");
        Serial.println(totalpulsos);
        Serial.print("WIND VELOCITY m/s=");
        Serial.println(velocidad,4);
        totalpulsos=0;   
    }

    // IF there is a Xbee message, then read it
    if(SerialXbee.available()){
      Serial.println("Receive Xbee message");
      serialXbee();
      Serial.println("Exit Xbee");
    }
    // IF there is a Bluetooth message, then read it
    if(Serial1.available()){
      Serial.println("Receive Bluetooth message");
      serialBluetooth();
      Serial.println("Exit Bluetooth");
    }
}

//---------------------------------------------------------
// Read soil moisture sensor
//---------------------------------------------------------
int SensorRead(int port_ON, int port_adc, int stab_time){
  int rawData;

  digitalWrite(port_ON, HIGH);
  delay(stab_time);
  rawData = analogRead(port_adc);
  delay(20);
  digitalWrite(port_ON, LOW);
  delay(20);
  
  return rawData;
}

//---------------------------------------------------------
// Calculate soil moisture from sensor reading
//---------------------------------------------------------
float MoistureCalculation(int SensorRaw, char eq_num, int adc_resolution, float voltage_reference){
    float moisture;
    float milivolts;
    float milivolts_resolution;
    
    milivolts_resolution = float((voltage_reference/(pow(2,adc_resolution) - 1))*1000);
    milivolts = float(SensorRaw * milivolts_resolution);
       
    switch(eq_num){
        case '0':     /* Factory Calibration */
        moisture = ((0.00119*milivolts) - 0.4000)*100;  
        break;
        
        case '1':     /* Area TEC EC5 */
        moisture = ((0.0009*milivolts) - 0.3487)*100;  
        break;
        
        case '2':     /* Area TEC 10HS */
        moisture = ((0.0008*milivolts) - 0.4138)*100;
        break;
        
        case '3':     /* Area Delicias EC5 */
        moisture = ((0.0009*milivolts) - 0.3926)*100;  
        break;
        
        case '4':     /* Area Delicias 10HS */
        moisture = ((0.0008*milivolts) - 0.4897)*100;
        break;
        
        case '5':     /* Area GranjaHogar EC5 */
        moisture = ((0.0009*milivolts) - 0.3487)*100;  
        break;
        
        case '6':     /* Area GranjaHogar 10HS */
        moisture = ((0.0008*milivolts) - 0.3681)*100;
        break;

        default:     /* Factory Calibration */
        moisture = ((0.00119*milivolts) - 0.4000)*100;  
        break;
    }
    if (moisture < 0.0) {
        moisture=0.0;      
        }
        
    return moisture;
}

//---------------------------------------------------------
// Process Xbee message and send a response
//---------------------------------------------------------
void serialXbee(){
      while(!SerialXbee.available());   
      SerialXbee.readBytes(Buffin,input_message_size);
      if(Buffin[0] == 0x7E && Buffin[15] == 'S'){ 
            soil_moisture_equation=Buffin[18];     
          /*=======Emisor MAC address=======*/
            XBeeAdd[0] = Buffin[4];
            XBeeAdd[1] = Buffin[5];
            XBeeAdd[2] = Buffin[6];
            XBeeAdd[3] = Buffin[7];
            XBeeAdd[4] = Buffin[8];
            XBeeAdd[5] = Buffin[9];
            XBeeAdd[6] = Buffin[10];
            XBeeAdd[7] = Buffin[11];
            /*=================================*/
            /*=======API Frame=======*/
            Buffout[0]=0x7E;                 //API mode start delimeter
            Buffout[1]=0x00;                 //Data length MSB
            Buffout[2]=output_message_size-4;//Data length LSB
            Buffout[3]=0x10;            //API ID TX Request 64 bit address
            Buffout[4]=0x00;            //Frame ID, if equals to 0x00 disable response frame
            Buffout[5]=XBeeAdd[0];      //Destination Address MSB
            Buffout[6]=XBeeAdd[1];            
            Buffout[7]=XBeeAdd[2];
            Buffout[8]=XBeeAdd[3];
            Buffout[9]=XBeeAdd[4];
            Buffout[10]=XBeeAdd[5];
            Buffout[11]=XBeeAdd[6];
            Buffout[12]=XBeeAdd[7];     //Destination Address LSB                
            Buffout[13]=0xFF;
            Buffout[14]=0xFE;
            Buffout[15]=0x00;
            Buffout[16]=0x00;
            Buffout[17]=NODE_TYPE;           // Node 'S'
            Buffout[18]=S1_MOIST_INT;   //Moisture 1 integer
            Buffout[19]=S1_MOIST_DEC;   //Moisture 1 decimals
            Buffout[20]=S2_MOIST_INT;   //Moisture 2 integer
            Buffout[21]=S2_MOIST_DEC;   //Moisture 2 decimals
            Buffout[22]=S3_MOIST_INT;   //Moisture 3 integer
            Buffout[23]=S3_MOIST_DEC;   //Moisture 3 decimals
            Buffout[24]=NDVI_INT;       //NDVI integer
            Buffout[25]=NDVI_DEC;       //NDVI decimals
            Buffout[26]=0x00;           //Not used
            Buffout[27]=0x00;           //Not used
            Buffout[28]=0x00;           //Not used
            Buffout[29]=0x00;           //Not used
            Buffout[30]=0x0A;           //End of packet
            for(int i=3;i<output_message_size-1;i++){      
                  sum1+=Buffout[i]; //Checksum
            }
            Buffout[31]=0xFF-lowByte(sum1);
            sum1=0;
            SerialXbee.write(Buffout,output_message_size);
      }
}


//---------------------------------------------------------
// Process Bluetooth message and send a response
//---------------------------------------------------------
void serialBluetooth(){
  char bt_message_in[3];

  while(!Serial1.available());   
  Serial1.readBytes(bt_message_in,3);
  delay(100);
  while(Serial1.available()){
    Serial1.read();   
  }
  delay(100);
  if(bt_message_in[0]=='s' && bt_message_in[1]=='e' && bt_message_in[2]=='n'){
    Serial1.println("SM");
    delay(100);
    Serial1.println(S1_MOIST);
    delay(100);
    Serial1.println(S2_MOIST);
    delay(100);
    Serial1.println(S3_MOIST);
    delay(100);
    Serial1.println("NDVI");
    delay(100);
    Serial1.println(red_data,5);
    delay(100);
    Serial1.println(nir_data,5);
    delay(100);
    Serial1.println(ndvi_data,5);
    delay(100);
    Serial1.println("WIND");
    delay(100);
    Serial1.println(velocidad);
    delay(100);
    Serial1.println(totalpulsos);
    delay(100);
  }
}

//---------------------------------------------------------
// read NDVI data using  SerialSDI_1
//---------------------------------------------------------
void readNDVI(){
    char dato1[7], dato2[7];
    String message;
    int i;

    //Energizar sensor
    digitalWrite(SDI1_ON, HIGH);
    delay(500);
    
    //Esperar y leer datos provenientes del sensor
    if (SerialSDI_1.available()) {   

        message=SerialSDI_1.readString();

        dato1[0] = message[2];
        dato1[1] = message[3];
        dato1[2] = message[4];
        dato1[3] = message[5];
        dato1[4] = message[6];
        dato1[5] = message[7];
        dato1[6] = 0;
 
        dato2[0] = message[9];
        dato2[1] = message[10];
        dato2[2] = message[11];
        dato2[3] = message[12];
        dato2[4] = message[13];
        dato2[5] = message[14];
        dato2[6] = 0;

        sensor_orientation=message[16];
        sensor_type=message[18];

        //Convertir arreglo "a" a variable flotante
        red_data = atof(dato1); 
        //Convertir arreglo "b" a flotante 
        nir_data = atof(dato2);
    }
    
    //Desenergizar sensor
    delay(50);
    digitalWrite(SDI1_ON, LOW);
}


//---------------------------------------------------------
// ISR for External Interrupt (A1/16) or (A2/17) to read wind sensor pulses
//---------------------------------------------------------
void readWind(){ 
  if ((millis() - ContactBounceTime) > 15 ) { // debounce the switch contact.
    totalpulsos=totalpulsos+1;
    ContactBounceTime = millis();
  }
}


//---------------------------------------------------------
// Attach the interrupt handler to the SERCOM
//---------------------------------------------------------
void SERCOM1_Handler()
{
  SerialXbee.IrqHandler();
}

//---------------------------------------------------------
// Attach the interrupt handler to the SERCOM
//---------------------------------------------------------
void SERCOM3_Handler()
{
  SerialSDI_1.IrqHandler();
}

//---------------------------------------------------------
// Attach the interrupt handler to the SERCOM
//---------------------------------------------------------
void SERCOM0_Handler()
{
  SerialSDI_2.IrqHandler();
}

