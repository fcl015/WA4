
/*========================================================================*/
/*=====                   Sensor node firmware v4.0                       */
/*=====                          ARDUINO MKR                              */
/*=====                        Water Automation                           */
/*=====                                                                   */
/*=====     To power SM sensors use digital pins 4, 5 and 6               */
/*=====     To read SM sensors use analog pins A4, A5, A6                 */
/*=====     To power NDVI sensor use digital pins 7                       */
/*=====     To read NDVI sensor use serial pin (RX) 3                     */
/*=====     To power LED2 use A3 as a digital output                      */
/*========================================================================*/
#include <math.h> 
#include <Arduino.h>        // required before wiring_private.h
#include "wiring_private.h" // pinPeripheral() function

/*=====Arduino definitions=====*/
#define NODE_TYPE           'S'
#define S1_ON               4   // Pulses to activate SM Sensors 4,5,6
#define S2_ON               5
#define S3_ON               6
#define S1_PORT             A4  // Analog read from SM Sensors A4, A5, A6
#define S2_PORT             A5
#define S3_PORT             A6
#define SDI1_ON             10  // Pulse to activate VP3 Sensor 10 (NOT USED FOR SENSOR NODE)
#define SDI2_ON             7   // Pulse to activate NDVI Sensors 7
#define LED_SIGNAL          A3  // A3 as an indicator led
#define SAMPLING_PERIOD     60  //Sampling period in seconds 
#define TIME_OFFSET         10  //Time Offset to execute first measurement in seconds

#define ADC_RESOLUTION      12  //12 bits ADC resolution
#define SET_VOLTAGE_REF()   analogReference(AR_INTERNAL1V65) // Internal Analog Reference 1.65 volts
#define VOLTAGE_REF         1.65  // Internal Analog Reference 1.65 volts
#define INITIAL_SM_EQN      '1'   // '0'-Default EC5, '1'-Default 10HS, '2'-AreaTec EC5, '3'-GranjaHogar 10HS 
                                  // '4'-GranjaHogar 10HS OLD, '5'-AreaTec 10HS OLD (Value overwritten by Zigbee communication)
#define INITIAL_ALPHA       1.86  // Initial alpha value (Value overwritten by Zigbee communication)
#define SM_PULSE_MS         500 // Pulse to read SM=100ms
#define NDVI_PULSE_MS       500 // Pulse to read NDVI=500ms
#define POST_PULSE_MS       50  // Post pulse 50ms
#define NDVI_MESSAGE_SIZE   23 // Message size NDVI
#define TURN_ON_LED()       digitalWrite(LED_SIGNAL,HIGH)
#define TURN_OFF_LED()      digitalWrite(LED_SIGNAL,LOW)


/*=====Timer variables=====*/
unsigned long previousMillis;
unsigned long timeInterval;
unsigned long sequence=0;
int debugMode = 1;
/*=========================*/

/*======Soil Moisture Variables========*/
int s1_raw, s2_raw, s3_raw;
float s1_moist, s2_moist, s3_moist;
char soil_moisture_equation=INITIAL_SM_EQN;

/*======NDVI Variables========*/
float red_data=0.0;
float nir_data=0.0;
float ndvi_data=0.0;
float alpha_data=INITIAL_ALPHA;
char sensor_type=0;
char sensor_orientation=0;
char ndvi_message[NDVI_MESSAGE_SIZE];
char ndvi_error_code=0;      // 0 - No error, 1 - Wrong sensor or wrong orientation, 2 - No data availble

/*=====Communication variables=====*/
char input_message_size=22;
char output_message_size=32;
char Buffout[80], Buffin[80];
int  sum1;
/*=================================*/

//---------------------------------------------------------
// Create the new UART instance for Xbee communication assigning it to pin 8(TX) and 9(RX)
//---------------------------------------------------------
Uart SerialXbee(&sercom1, 8, 9, SERCOM_RX_PAD_1, UART_TX_PAD_0);
//---------------------------------------------------------
// Create the new UART instance for SDI_1 assigning it to pin 0(TX) and 1(RX) (NOT USED FOR SENSOR NODE)
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
  
  SerialSDI_1.begin(1200); // Configure Pins 0(TX),1(RX) for SDI_1 communication (NOT USED FOR SENSOR NODE)
  while(!SerialSDI_1);
  pinPeripheral(0, PIO_SERCOM);  
  pinPeripheral(1, PIO_SERCOM);  
  
  SerialSDI_2.begin(1200); // Configure Pins 2(TX),3(RX) for SDI_2 communication (NDVI Sensor)
  while(!SerialSDI_2);
  pinPeripheral(2, PIO_SERCOM);  
  pinPeripheral(3, PIO_SERCOM);  

  pinMode(S1_ON, OUTPUT);
  pinMode(S2_ON, OUTPUT);
  pinMode(S3_ON, OUTPUT);
  pinMode(SDI1_ON, OUTPUT);
  pinMode(SDI2_ON, OUTPUT);
  pinMode(LED_SIGNAL, OUTPUT);

  analogReadResolution(ADC_RESOLUTION); // Configure 12 bit ADC     
  SET_VOLTAGE_REF();                    // Configure Reference ADC to 1.0 or 1.65 volts
  timeInterval = TIME_OFFSET*1000;     // Initial interval time 5 seconds 
  initialSequence();                    // Execute initial sequence of led2
}

//---------------------------------------------------------
// Main loop
//---------------------------------------------------------
void loop(){ 
   unsigned long currentMillis = millis();
 
    // IF TimeInterval is over, then calculate soil moisture and NDVI
    if(currentMillis - previousMillis > timeInterval) {
        // Update timeinterval
        TURN_ON_LED();
        previousMillis = currentMillis;   
        // Obtain Soil Moisture Sensors
        readSM();
        // Obtain NDVI
        // readNDVI();
        // Print data for debugging
        if(debugMode==1){
            sequence=sequence+1;
            printDebugData();
        }
        timeInterval = SAMPLING_PERIOD*1000;
        TURN_OFF_LED();
    }
    // IF there is a Xbee message, then read it
    if(SerialXbee.available()){
        TURN_ON_LED();
        serialXbee();
        TURN_OFF_LED();
    }
    // IF there is a Bluetooth message, then read it
    if(Serial1.available()){
        TURN_ON_LED();
        serialBluetooth();
        TURN_OFF_LED();
    }
}


//---------------------------------------------------------
// Initial sequence to identify sensor type
//---------------------------------------------------------
void initialSequence(){
    TURN_ON_LED();
    delay(200);
    TURN_OFF_LED();    
    delay(500);
    TURN_ON_LED();
    delay(200);
    TURN_OFF_LED();    
    delay(500);
    TURN_ON_LED();
    delay(200);
    TURN_OFF_LED();    
    delay(500);
}
//---------------------------------------------------------
// Read soil moisture sensors
//---------------------------------------------------------
void readSM(){
  
  // Read SM Analog Values
  s1_raw = SensorRead(S1_ON, S1_PORT, SM_PULSE_MS);
  s2_raw = SensorRead(S2_ON, S2_PORT, SM_PULSE_MS);
  s3_raw = SensorRead(S3_ON, S3_PORT, SM_PULSE_MS);
}

//---------------------------------------------------------
// Read soil moisture analog values
//---------------------------------------------------------
int SensorRead(int port_ON, int port_adc, int pulse_delay){
    int rawData;

    digitalWrite(port_ON, HIGH);
    delay(pulse_delay);
    rawData = analogRead(port_adc);
    delay(POST_PULSE_MS);
    digitalWrite(port_ON, LOW);
  
    return rawData;
}

//---------------------------------------------------------
// Calculate soil moisture from sensor reading
//---------------------------------------------------------
float MoistureCalculation(int SensorRaw){
    float moisture;
    float milivolts;
    float milivolts_resolution;
    
    milivolts_resolution = float((VOLTAGE_REF/(pow(2,ADC_RESOLUTION) - 1))*1000);
    milivolts = float(SensorRaw * milivolts_resolution);
       
    switch(soil_moisture_equation){
        case '0':     /* EC5 Factory Calibration */
          moisture = ((0.00119*milivolts) - 0.4010)*100;  
          break;
        
        case '1':     /* 10HS Factory Calibration */
          moisture = (2.97e-9*pow(milivolts,3)-7.37e-6*pow(milivolts,2)+6.69e-3*milivolts-1.92)*100;  
          break;

        case '2':     /* Area TEC EC5 */
          moisture = ((0.0009*milivolts) - 0.3487)*100;  
          break;
        
        case '3':     /* Area Granja Hogar 10HS */
          moisture = ((0.000546*milivolts) - 0.284170)*100;
          break;

        case '4':     /* Area GranjaHogar 10HS OLD*/
          moisture = ((0.0008*milivolts) - 0.3681)*100;
          break;

        case '5':     /* Area TEC 10HS OLD*/
          moisture = ((0.0008*milivolts) - 0.4138)*100;
          break;

       default:     /* If not defined assign 99.99 */
          moisture = 99.99;  
          break;
    }
    if (moisture < 0.0) {
        moisture=0.0;      
        }   
    return moisture;
}

//---------------------------------------------------------
// read NDVI data using  SerialSDI_2
//---------------------------------------------------------
void readNDVI(){
    char dato1[7], dato2[7];
    int i;

    //Energizar sensor
    digitalWrite(SDI2_ON, HIGH);
    delay(NDVI_PULSE_MS);
    
    //Empty message buffer
    for(int i=0; i<NDVI_MESSAGE_SIZE; i++){
          ndvi_message[i]=0;     
    }

    //Esperar y leer datos provenientes del sensor
    if (SerialSDI_2.available()) {   
        SerialSDI_2.readBytes(ndvi_message,NDVI_MESSAGE_SIZE);

        // Extract red and nir and ndvi from string
        int state=0;
        int j=0;
        for(int i=0; i<NDVI_MESSAGE_SIZE; i++){
              switch(state){
                case 0:   if(ndvi_message[i]==0x09){   // 0x09 Tab character signaling the start of the data transmission
                            state++;
                          }
                          break;
                case 1:   if(ndvi_message[i]==' '){   // Space character separate red data from nir data
                            state++;
                            dato1[j]=0;
                            j=0;
                          }
                          else{                 // Capture red data until a space character is found
                            dato1[j]=ndvi_message[i];
                            j++;
                          }
                          break;
                case 2:   if(ndvi_message[i]==' '){   // Space character separate nir data from sensor orientation
                            state++;
                            dato2[j]=0;
                            j=0;
                          }
                          else{                 // Capture nir data until a space character is found
                            dato2[j]=ndvi_message[i];
                            j++;
                          }
                          break;
                case 3:   sensor_orientation=ndvi_message[i];  // Capture sensor orientation byte
                          sensor_type=ndvi_message[i+2];       // Capture sensor type
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
    
    //Empty serial input buffer
    while(SerialSDI_2.available()){
            SerialSDI_2.read();
    }
    //Desenergizar sensor
    delay(POST_PULSE_MS);
    digitalWrite(SDI2_ON, LOW);
}


//---------------------------------------------------------
// Print debug data
//---------------------------------------------------------
void printDebugData(){
    Serial.println("--------------------------");
    Serial.println(sequence);
    Serial.println("--------------------------");
    Serial.print("S1_RAW=");
    Serial.println(s1_raw);
    Serial.print("S2_RAW=");
    Serial.println(s2_raw);
    Serial.print("S3_RAW=");
    Serial.println(s3_raw);

    // Calculate SM from Analog Vaues 
    s1_moist = MoistureCalculation(s1_raw);
    s2_moist = MoistureCalculation(s2_raw);
    s3_moist = MoistureCalculation(s3_raw);
    Serial.println(soil_moisture_equation);    
    Serial.print("S1_VWC=");
    Serial.println(s1_moist,2);
    Serial.print("S2_VWC=");
    Serial.println(s2_moist,2);
    Serial.print("S3_VWC=");
    Serial.println(s3_moist,2);

    Serial.print("NDVI RAW MESSAGE :"); 
    for(int i=0; i<NDVI_MESSAGE_SIZE; i++){
          Serial.print(ndvi_message[i]);      
    }
    Serial.print("NDVI MESSAGE HEX :");
    for(int i=0; i<NDVI_MESSAGE_SIZE; i++){
          Serial.print(ndvi_message[i],HEX);      
          Serial.print(' '); 
    }
    Serial.println(' ');  
    Serial.print("NDVI ERROR CODE :");
    Serial.println(ndvi_error_code,DEC);
    Serial.print("RED=");
    Serial.println(red_data,4);
    Serial.print("NIR=");
    Serial.println(nir_data,4);              
    Serial.print("ALPHA=");
    Serial.println(alpha_data,4);
    Serial.print("NDVI=");
    Serial.println(ndvi_data,4);  
}

//---------------------------------------------------------
// Process Xbee message and send a response
//---------------------------------------------------------
void serialXbee(){
      
      while(!SerialXbee.available());   
      SerialXbee.readBytes(Buffin,input_message_size);
      if(Buffin[0] == 0x7E && Buffin[15] == NODE_TYPE){ 
            if (debugMode==1){
                Serial.println("Receive Xbee message");
            }
            soil_moisture_equation=Buffin[16];  
            if( Buffin[17]!=0 && Buffin[18]!=0 ){
                alpha_data=((float)(Buffin[17]*256+Buffin[18]))/10000.0; 
            }
            /*=================================*/
            /*=======API Frame=======*/
            Buffout[0]=0x7E;                  //API mode start delimeter
            Buffout[1]=0x00;                  //Data length MSB
            Buffout[2]=output_message_size-4; //Data length LSB
            Buffout[3]=0x10;                  //API ID TX Request 64 bit address
            Buffout[4]=0x00;                  //Frame ID, if equals to 0x00 disable response frame
            Buffout[5]=Buffin[4];             //Destination Address MSB
            Buffout[6]=Buffin[5];            
            Buffout[7]=Buffin[6];
            Buffout[8]=Buffin[7];
            Buffout[9]=Buffin[8];
            Buffout[10]=Buffin[9];
            Buffout[11]=Buffin[10];
            Buffout[12]=Buffin[11];           //Destination Address LSB                
            Buffout[13]=0xFF;
            Buffout[14]=0xFE;
            Buffout[15]=0x00;
            Buffout[16]=0x00;
            Buffout[17]=NODE_TYPE;            // Node 'S'
 
            // Calculate SM from Analog Vaues 
            s1_moist = MoistureCalculation(s1_raw);
            s2_moist = MoistureCalculation(s2_raw);
            s3_moist = MoistureCalculation(s3_raw);

            Buffout[18]=highByte(int(s1_moist*100)); //SM1 high byte
            Buffout[19]=lowByte(int(s1_moist*100));  //SM1 low byte 
            Buffout[20]=highByte(int(s2_moist*100)); //SM2 high byte
            Buffout[21]=lowByte(int(s2_moist*100));  //SM2 low byte
            Buffout[22]=highByte(int(s3_moist*100)); //SM3 high byte
            Buffout[23]=lowByte(int(s3_moist*100));  //SM3 low byte
            Buffout[24]=highByte(int(ndvi_data*10000));  //NDVI high byte
            Buffout[25]=lowByte(int(ndvi_data*10000));   //NDVI low byte
            Buffout[26]=highByte(int(red_data*10000));  //RED high byte
            Buffout[27]=lowByte(int(red_data*10000));   //RED low byte
            Buffout[28]=highByte(int(nir_data*10000));  //NIR high byte
            Buffout[29]=lowByte(int(nir_data*10000));   //NIR low byte
           
            Buffout[30]=ndvi_error_code;               //Error code

            for(int i=3;i<output_message_size-1;i++){      
                  sum1+=Buffout[i]; //Checksum
            }
            Buffout[31]=0xFF-lowByte(sum1);
            sum1=0;
            SerialXbee.write(Buffout,output_message_size);
            if (debugMode==1){
                Serial.println("Exit Xbee");
            }
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
    if (debugMode==1){
      Serial.println("Receive Bluetooth message");
    }
    readSM();
    Serial1.println("SM RAW");
    delay(100);
    Serial1.println(s1_raw);
    delay(100);
    Serial1.println(s2_raw);
    delay(100);
    Serial1.println(s3_raw);
    delay(100);
    Serial1.println("SM VWC");
    delay(100);
    Serial1.println(soil_moisture_equation);    
    delay(100);
    // Calculate SM from Analog Vaues 
    s1_moist = MoistureCalculation(s1_raw);
    s2_moist = MoistureCalculation(s2_raw);
    s3_moist = MoistureCalculation(s3_raw);
    Serial1.println(s1_moist);
    delay(100);
    Serial1.println(s2_moist);
    delay(100);
    Serial1.println(s3_moist);
    delay(100);
    //Serial1.println("NDVI");
    //delay(100);
    //Serial1.println(red_data,5);
    //delay(100);
    //Serial1.println(nir_data,5);
    //delay(100);
    //Serial1.println(ndvi_data,5);
    //delay(100);
    if (debugMode==1){
      Serial.println("Exit Bluetooth");
    }
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
