
/*===================================================================================================================================*/
/*=====                                          Sensor+Weather node firmware v4.0                                              =====*/
/*=====                                                   ARDUINO MKR                                                           =====*/
/*=====                                              Water Automation                                                           =====*/
/*=====                                                                                                                         =====*/
/*=====     To power SM sensors use digital pins 4, 5 and 6                                                                     =====*/
/*=====     To read SM sensors use analog pins A4, A5, A6                                                                       =====*/
/*=====     To power PYR SunRadiation sensor use digital pin 4                                                                  =====*/
/*=====     To read  PYR SunRadiation sensors use analog pin A4                                                                 =====*/
/*=====     To power NDVI sensor use digital pins 10                                                                            =====*/
/*=====     To power VP3 Temp/Hum sensor use digital pin 7                                                                      =====*/
/*=====     To read VP3 Tep/Hum sensor use serial pin (RX) 1                                                                    =====*/
/*=====     To read WIND DavisCup pulses use interrupt pin A1/INT16                                                              =====*/
/*=====     To read NDVI sensor use serial pin (RX) 3                                                                           =====*/
/*===================================================================================================================================*/
#include <math.h> 
#include <Arduino.h>        // required before wiring_private.h
#include "wiring_private.h" // pinPeripheral() function

/*=====Arduino definitions=====*/
#define NODE_TYPE_S     'S'
#define NODE_TYPE_W     'W'
#define S1_ON           4   // Pulses to activate SM Sensors 4,5,6
#define S2_ON           5
#define S3_ON           6
#define PYR             A4  // Analog read from SunRadiationSensor A4
#define S1_PORT         A4  // Analog read from SM Sensors A4, A5, A6
#define S2_PORT         A5
#define S3_PORT         A6
#define SDI1_ON         7   // Pulse to activate VP3 Sensor 7
#define SDI2_ON         10  // Pulse to activate NDVI Sensor 10
#define PULSE_IN        A1  // Digital input A1/INT16 or A2/INT17 to count pulses from WIND sensor
#define LED_SIGNAL      A0  // DAC as an indicator led
#define SAMPLING_PERIOD 60 //Sampling period in seconds 
#define TIME_OFFSET     20  //Time Offset to execute first measurement in seconds
#define ADC_RESOLUTION  12  //12 bits ADC resolution
#define VOLTAGE_REF     1.0 // 1.0 volts as reference
#define PULSE_DEBOUNCE  5   // Debounce delay=5ms
#define SM_PULSE_MS     100 // Pulse to read SM=100ms
#define PYR_PULSE_MS    100 // Pulse to read PYR=100ms
#define VP3_PULSE_MS    500 // Pulse to read VP3=500ms
#define NDVI_PULSE_MS   500 // Pulse to read NDVI=500ms
#define POST_PULSE_MS   50  // Post pulse 50ms
#define NDVI_MESSAGE_SIZE 23 // Message size NDVI
#define VP3_MESSAGE_SIZE  17 // Messgae size VP3
#define TURN_ON_LED()   analogWrite(LED_SIGNAL,255)
#define TURN_OFF_LED()  analogWrite(LED_SIGNAL,0)

/*=====Timer variables=====*/
unsigned long previousMillis;
unsigned long timeInterval;
int debugMode = 1;
/*=========================*/

/*======Soil Moisture Variables========*/
int s1_raw, s2_raw, s3_raw;
float s1_moist, s2_moist, s3_moist;
char soil_moisture_equation='1';

/*======Sun Radiation PYR Variables========*/
volatile int radiation;
int raw_radiation;

/*======Davis Cup WIND Variables========*/
volatile unsigned int totalpulsos=0; 
volatile unsigned int totalpulsos_ant=0; 
volatile unsigned int ContactBounceTime=0;
float totalpulsosrps;
float velocidad;

/*======VP3 Temp/Humidity Variables========*/
float humedad, temp; 
char vp3_message[VP3_MESSAGE_SIZE];
char vp3_sensor_type=0;
char vp3_error_code=0;      // 0 - No error, 4 - Wrong sensor or wrong orientation, 8 - No data availble

/*======NDVI Variables========*/
float red_data=0.0;
float nir_data=0.0;
float ndvi_data=0.0;
float alpha_data=1.86;
char sensor_type=0;
char sensor_orientation=0;
char ndvi_message[NDVI_MESSAGE_SIZE];
char ndvi_error_code=0;      // 0 - No error, 1 - Wrong sensor or wrong orientation, 2 - No data availble

/*======ETo Variables========*/
float ETo;
float Altitud=1440;   // Default value Chihuahua´s altitude
char current_month=5; // Default value 5-June
// Ra - Radiacion solar extraterrestre para latitud 28o
//    Ene 22.2  Feb 27.1  Mar 32.4  Abr 37.2  May 39.9  Jun 40.9 
//    Jul 40.4  Ago 38.3  Sep 34.4  Oct 29.2  Nov 23.8  Dic 21.1
float Ra_data_array[12]={22.2, 27.1, 32.4, 37.2, 39.9, 40.9, 40.4, 38.3, 34.4, 29.2, 23.8, 21.1}; 
// G - Flujo calorifico utilizado en el calentamiento del suelo
//    Ene 0.077   Feb 0.441   Mar 0.651   Abr 0.630   May 0.448   Jun -0.091  
//    Jul -0.231  Ago -0.175  Sep -0.336  Oct -0.455  Nov -0.609  Dic -0.350
float G_data_array[12]={0.077,0.441,0.651,0.630,0.448,-0.091,-0.231,-0.175,-0.336,-0.455,-0.609,-0.350 };

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

  pinMode(PULSE_IN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PULSE_IN), readPulses, FALLING); // Interrupcion en pin A1/16 or A2/17 para conteo por hardware
  
  analogReadResolution(ADC_RESOLUTION); // Configure 12 bit ADC     
  analogReference(AR_INTERNAL1V0);// Configure Reference ADC to 1.0 volts

  analogWriteResolution(8);
  timeInterval = TIME_OFFSET*1000;     // Initial interval time 5 seconds 

  initialSequence();
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
        readNDVI();
        // Print data for debugging
        readWIND(); 
        // Obtain Temp/Hum 
        readVP3();
        // Obtain Sun Radiation
        readPYR();
        // Obtain ETo
        ETo = EtoCalc(radiation,humedad,temp,velocidad);
        if(debugMode==1){
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
    delay(2000);

    TURN_ON_LED();
    delay(200);
    TURN_OFF_LED();    
    delay(500);
    TURN_ON_LED();
    delay(1000);
    TURN_OFF_LED();    
    delay(500);
    TURN_ON_LED();
    delay(1000);
    TURN_OFF_LED();    
    delay(500);

    Serial1.println("SENSOR+WEATHER NODE");
    //Serial.println("SENSOR+WEATHER NODE");
}

//---------------------------------------------------------
// ISR for External Interrupt (A1/16) or (A2/17) to read wind sensor pulses
//---------------------------------------------------------
void readPulses(){ 
  if ((millis() - ContactBounceTime) > PULSE_DEBOUNCE ) { 
    totalpulsos=totalpulsos+1;
    ContactBounceTime = millis();
  }
}

//---------------------------------------------------------
// Read soil moisture sensors
//---------------------------------------------------------
void readSM(){
  
  // Read SM Analog Values
  s1_raw = SensorRead(S1_ON, S1_PORT, SM_PULSE_MS);
  s2_raw = SensorRead(S2_ON, S2_PORT, SM_PULSE_MS);
  s3_raw = SensorRead(S3_ON, S3_PORT, SM_PULSE_MS);
  
  // Calculate SM from Analog Vaues 
  s1_moist = MoistureCalculation(s1_raw, soil_moisture_equation, ADC_RESOLUTION, VOLTAGE_REF);
  s2_moist = MoistureCalculation(s2_raw, soil_moisture_equation, ADC_RESOLUTION, VOLTAGE_REF);
  s3_moist = MoistureCalculation(s3_raw, soil_moisture_equation, ADC_RESOLUTION, VOLTAGE_REF);
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
float MoistureCalculation(int SensorRaw, char eq_num, int adc_resolution, float voltage_reference){
    float moisture;
    float milivolts;
    float milivolts_resolution;
    
    milivolts_resolution = float((VOLTAGE_REF/(pow(2,ADC_RESOLUTION) - 1))*1000);
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
        //Calcular NVDI si el sensor tiene orientacion hacia abajo '1' , tipo de sensor puntual 'r'
        if( sensor_type=='r' and sensor_orientation=='1' ){
              ndvi_data = (alpha_data*nir_data - red_data) / (alpha_data*nir_data + red_data);
              ndvi_error_code=0;
        }
        //Calcular NVDI si el sensor tiene orientacion hacia arriba '2' , tipo de sensor de hemisferico 's'
        else if( sensor_type=='s' and sensor_orientation=='2' ){
              alpha_data = red_data / nir_data;
              ndvi_data = 0.0;
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
// Read WIND Davis Cup Anemometer sensor
//---------------------------------------------------------
void readWIND(){
    totalpulsosrps = float(totalpulsos)/(timeInterval/1000);
    velocidad=2.25*0.44704*totalpulsosrps;
    totalpulsos_ant=totalpulsos;
    totalpulsos=0;   
}

//---------------------------------------------------------
// Read PYR Sun Radiation sensor
//---------------------------------------------------------
void readPYR(){
    float tmp_rad;
    
    //Energizar sensor
    digitalWrite(S1_ON, HIGH);
    delay(PYR_PULSE_MS);
    //Lectura analógica del sensor
    raw_radiation=analogRead(PYR);
    //Conversion a radiación solar watt/m2
    // W/m2 = RAW * (1500/4096)*5.0
    // W/m2 = RAW * (adc_ref_milliVolts/adc_resolution)*5.0
    tmp_rad = (float)raw_radiation*(1000.0/4096.0)*5.0;  //ADC reference 1.0 volts, ADC resolution 4096
    radiation = (int)tmp_rad;
    //Desenergizar sensor
    digitalWrite(S1_ON, LOW);  
}


//---------------------------------------------------------
// Read VP3 Temp/Hum Sensor using  SerialSDI_1
//---------------------------------------------------------
void readVP3(){
    char dato1[7], dato2[7];
    int i;

    //Energizar sensor
    digitalWrite(SDI1_ON, HIGH);
    delay(VP3_PULSE_MS);

    //Empty message buffer
    for(int i=0; i<VP3_MESSAGE_SIZE; i++){
          vp3_message[i]=0;     
    }

    //Esperar y leer datos provenientes del sensor
    if (SerialSDI_1.available()) {   
        SerialSDI_1.readBytes(vp3_message,VP3_MESSAGE_SIZE);

        // Extract vapor pressure and temperature from string
        int state=0;
        int j=0;
        for(int i=0; i<VP3_MESSAGE_SIZE; i++){
              switch(state){
                case 0:   if(vp3_message[i]==0x09){   // 0x09 Tab character signaling the start of the data transmission
                            state++;
                          }
                          break;
                case 1:   if(vp3_message[i]==' '){   // Space character separate vapor pressure dta from temperature data
                            state++;
                            dato1[j]=0;
                            j=0;
                          }
                          else{                 // Capture vapor pressure data until a space character is found
                            dato1[j]=vp3_message[i];
                            j++;
                          }
                          break;
                case 2:   if(vp3_message[i]==0x0D){ // 0x0D This carriage return character signals the end of the measurement string
                            state++;
                            dato2[j]=0;
                            vp3_sensor_type=vp3_message[i+1];
                            j=0;
                          }
                          else{                 // Capture temperture data until 0x0D character is found
                            dato2[j]=vp3_message[i];
                            j++;
                          }
                          break;
                defult:   break;
              }
        }
        if( state>2 ){
                if( vp3_sensor_type=='p' ){ 
                      //Convertir arreglo "dato2" a variable flotante
                      temp = atof(dato2); 
                      //Convertir arreglo "dato1" a flotante y obtener humedad relativa a partir de la presión de vapor y la temperatura
                      humedad = (atof(dato1)/(0.611*pow(2.7182,((17.502*temp)/(240.97+temp))))*100);
                      vp3_error_code=0;
                }
                else{
                      vp3_error_code=4;
                }
        }
        else{
              temp=0.0;
              humedad=0.0;
              vp3_error_code=8;
        }
    }
    
    //Empty serial input buffer
    while(SerialSDI_1.available()){
      SerialSDI_1.read();
    }
    //Desenergizar sensor
    delay(POST_PULSE_MS);
    digitalWrite(SDI1_ON, LOW);
}

//---------------------------------------------------------
// Calculate ETo using Penman-Moteith
//---------------------------------------------------------
// Evapotranspiration method: Penman-Monteith FAO 1998
// EQUATION:
//                                             900  
//          0.408 delta ( Rn - G ) + gamma ---------- u2 ( es - ea)
//                                            T + 273
//   ETo = ------------------------------------------------------------
//                     delta + gamma ( 1 + 0.34 u2 ) 
//   where
//       delta - pendiente de la curva de la tension de vapor saturado (kPa/oC)
//       Rn - radiacion neta sobre la superficie de cultivo (MJ/(m^2 day))
//       G - flujo calorifico utilizado en el calentamiento del suelo (MJ/(m^2 day))
//       gamma - constante psicometrica (kPa/oC)
//       u2 - velocidad del viento a 2 metros de altura (m/s)
//       es-ea - deficit de tension de vapor (kPa)
//  
//
// En periodo horario: Se entra con los datos medios horarios y el resultado de
// ETo se cambia 0,34, por 0,24 en el día y por 0,96,
// en la noche.
//
//---------------------------------------------------------

float EtoCalc(float Rad_Eto, float Hum_Eto, float Temp_Eto, float Viento_Eto){
    float eto_mm_day;
    float Ra = Ra_data_array[current_month];
    float G = G_data_array[current_month];
    
    //Pendiente de la curva de la tensión de vapor saturado (kPa/ ºC)
    float delta = (2503.6*pow(2.7182,(17.27*Temp_Eto/(Temp_Eto+237.3))))/pow((Temp_Eto+237.3),2); 
    //Tension de vapor saturado a una temperatura dada en Kpa
    float es = 0.6108*pow(2.7182,(17.27*Temp_Eto/(Temp_Eto+237.3)));                              
    //Tension de vapor actual en Kpa
    float ea = es*Hum_Eto/100.0; 
    
    //Estimacion de Rn a partir de Ra según Hargreaves en MJ/(m^2 dia)
    float Rs=Rad_Eto*0.0864;      // Radiación solar en MJ/(m^2 dia)
    float alfa=0.23;              // albedo de la grama =0.23
    float sigma=4.903e-9;         // constante de Stefan-Boltzmann
    float Tkelvin=Temp_Eto+273;   // Temperatura en Kelvin
    float Rso=Ra*(2e-5*Altitud+0.75);
    float Rn=Rs*(1-alfa)-sigma*pow(Tkelvin,4)*(0.34-0.14*sqrt(ea))*((1.35*Rs/Rso)-0.35);
    float coef_u2;
    if( Rs > 0){
        coef_u2=0.24;   //0.24 dia
    }
    else{
        coef_u2=0.96;   //0.96 noche
    }

    // Velocidad del viento en m/s
    float u2 = Viento_Eto;
    // Presion atmosferica en Kpa
    float P=101.3*pow(((293-0.0065*Altitud)/293),5.26); //Presion atmosferica en Kpa
    // Constante psicrométrica (kPa/ ºC)
    float gamma = 0.000665*P;
    
    // Calculo de ETo por metodo de Penman-Monteith
    eto_mm_day= (0.408*delta*(Rn-G)+gamma*(900/(Temp_Eto+273))*u2*(es-ea))/(delta+gamma*(1+coef_u2*u2));
    return eto_mm_day;
}

//---------------------------------------------------------
// Print debug data
//---------------------------------------------------------
void printDebugData(){
    Serial.println("---- SENSOR NODE ----");
    //Serial.print("S1_RAW=");
    //Serial.println(s1_raw);
    //Serial.print("S2_RAW=");
    //Serial.println(s2_raw);
    //Serial.print("S3_RAW=");
    //Serial.println(s3_raw);

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
    //Serial.print("NDVI MESSAGE HEX :");
    //for(int i=0; i<NDVI_MESSAGE_SIZE; i++){
    //      Serial.print(ndvi_message[i],HEX);      
    //      Serial.print(' '); 
    //}
    //Serial.println(' ');  
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

    Serial.println("---- WEATHER NODE ----");
    //Serial.print("RAW_RADIATION=");
    //Serial.println(raw_radiation);
    Serial.print("RADIATION=");
    Serial.println(radiation);
    
    //Serial.print("TOTAL_PULSOS=");
    //Serial.println(totalpulsos_ant);
    Serial.print("WIND VELOCITY=");
    Serial.println(velocidad,2);

    Serial.print("VP3 RAW MESSAGE :"); 
    for(int i=0; i<VP3_MESSAGE_SIZE; i++){
          Serial.print(vp3_message[i]);      
    }
    //Serial.print("VP3 MESSAGE HEX :");
    //for(int i=0; i<VP3_MESSAGE_SIZE; i++){
    //      Serial.print(vp3_message[i],HEX);      
    //      Serial.print(' '); 
    //}
    //Serial.println(' ');  
    Serial.print("VP3 ERROR CODE :");
    Serial.println(vp3_error_code,DEC);
    Serial.print("HUMIDITY=");
    Serial.println(humedad,2);
    Serial.print("TEMPERATURE=");
    Serial.println(temp,2);
    
    Serial.print("ETO=");
    Serial.println(ETo,2);  

    //Serial.print("NDVI RAW MESSAGE :"); 
    //for(int i=0; i<NDVI_MESSAGE_SIZE; i++){
    //      Serial.print(ndvi_message[i]);      
    //}
    //Serial.print("NDVI MESSAGE HEX :");
    //for(int i=0; i<NDVI_MESSAGE_SIZE; i++){
    //      Serial.print(ndvi_message[i],HEX);      
    //      Serial.print(' '); 
    //}
    //Serial.println(' ');  
    //Serial.print("NDVI ERROR CODE :");
    //Serial.println(ndvi_error_code,DEC);
    //Serial.print("RED=");
    //Serial.println(red_data,4);
    //Serial.print("NIR=");
    //Serial.println(nir_data,4);              
    //Serial.print("ALPHA=");
    //Serial.println(alpha_data,4);
    Serial.println("--------------------------");
}

//---------------------------------------------------------
// Process Xbee message and send a response
//---------------------------------------------------------
void serialXbee(){
      
      while(!SerialXbee.available());   
      SerialXbee.readBytes(Buffin,input_message_size);
      if(Buffin[0] == 0x7E && Buffin[15] == NODE_TYPE_S){ 
            if (debugMode==1){
                Serial.println("Receive Xbee message SENSOR");
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
            Buffout[17]=NODE_TYPE_S;            // Node 'S'
 
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
                Serial.println("Exit Xbee SENSOR");
            }
      }
      else if(Buffin[0] == 0x7E && Buffin[15] == NODE_TYPE_W){ 
            if (debugMode==1){
                Serial.println("Receive Xbee message WEATHER");
            }
            current_month=Buffin[16];   // Get current month from message
            Altitud=(float)(Buffin[17]*256+Buffin[18]);  // Get city altitude from message
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
            Buffout[17]=NODE_TYPE_W;            // Node 'W'
            
            Buffout[18]= highByte(radiation); //Radiacion solar MSB
            Buffout[19]= lowByte(radiation); //Radiacion solar LSB           
            Buffout[20]=highByte(int(humedad*100));   //Humedad MSB
            Buffout[21]=lowByte(int(humedad*100));    //Humedad LSB
            Buffout[22]=highByte(int(temp*100));   //Temp MSB
            Buffout[23]=lowByte(int(temp*100));    //Temp LSB           
            Buffout[24]=highByte(int(velocidad*100));   //Velocidad de viento MSB
            Buffout[25]=lowByte(int(velocidad*100));     //Velocidad de viento LSB
            Buffout[26]=highByte(int(ETo*100));     //Evapotranspiración MSB
            Buffout[27]=lowByte(int(ETo*100));      //Evapotranspiración LSB
            Buffout[28]=highByte(int(alpha_data*10000));//ALPHA high byte
            Buffout[29]=lowByte(int(alpha_data*10000));//ALPHA low byte

            Buffout[30]=ndvi_error_code|vp3_error_code; //Error code
            
            for(int i=3;i<output_message_size-1;i++){      
                  sum1+=Buffout[i]; //Checksum
            }
            Buffout[31]=0xFF-lowByte(sum1);
            sum1=0;
            SerialXbee.write(Buffout,output_message_size);
            if (debugMode==1){
                Serial.println("Exit Xbee WEATHER");
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
      Serial.println("Receive Bluetooth message SENSOR");
    }
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
    Serial1.println(s1_moist);
    delay(100);
    Serial1.println(s2_moist);
    delay(100);
    Serial1.println(s3_moist);
    delay(100);
    Serial1.println("NDVI");
    delay(100);
    Serial1.println(red_data,5);
    delay(100);
    Serial1.println(nir_data,5);
    delay(100);
    Serial1.println(ndvi_data,5);
    delay(100);
    if (debugMode==1){
      Serial.println("Exit Bluetooth SENSOR");
    }
  }
  else if(bt_message_in[0]=='w' && bt_message_in[1]=='e' && bt_message_in[2]=='a'){
    if (debugMode==1){
      Serial.println("Receive Bluetooth message WEATHER");
    }
    Serial1.println("RADIATION");
    delay(100);
    Serial1.println(raw_radiation);
    delay(100);
    Serial1.println(radiation);
    delay(100);
    Serial1.println("WIND");
    delay(100);
    Serial1.println(totalpulsos_ant);
    delay(100);
    Serial1.println(velocidad);
    delay(100);
    Serial1.println("HUM/TEMP");
    delay(100);
    Serial1.println(humedad,2);
    delay(100);
    Serial1.println(temp,2);
    delay(100);
    Serial1.println("ETo");
    delay(100);
    Serial1.println(ETo,2);
    delay(100);
    Serial1.println("NDVI ALPHA");
    delay(100);
    Serial1.println(alpha_data,4);
    delay(100);
    if (debugMode==1){
      Serial.println("Exit Bluetooth WEATHER");
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

