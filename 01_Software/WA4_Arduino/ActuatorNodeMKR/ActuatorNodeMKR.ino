
/*========================================================================*/
/*=====                   Actuator node firmware v4.0                       */
/*=====                          ARDUINO MKR                              */
/*=====                        Water Automation                           */
/*=====                                                                   */
/*=====     To power LED2 use 10 as a digital output                      */
/*========================================================================*/


#include <math.h>
#include <Arduino.h>        // required before wiring_private.h
#include "wiring_private.h" // pinPeripheral() function
#include <jm_PCF8574.h>
#include <Wire.h>
#include <string.h>
#include "PeriodicTask.h"

/*=====Optional hardware=====*/
#define FSEN5_ENABLE        false
/*===========================*/

/*=====Arduino definitions=====*/
#define NODE_TYPE           'A'
#define LED_SIGNAL          10  // A3 as an indicator led
#define SAMPLING_PERIOD     60  //Sampling period in seconds 
#define TIME_OFFSET         10  //Time Offset to execute first measurement in seconds

#define FSEN1_SIGNAL        A1  //Flow sensor 1 INPUT pin
#define FSEN2_SIGNAL        A2  //Flow sensor 2 INPUT pin
#define FSEN3_SIGNAL         0  //Flow sensor 3 INPUT pin
#define FSEN4_SIGNAL         1  //Flow sensor 4 INPUT pin
#define FSEN5_SIGNAL         4  //Flow sensor 5 INPUT pin

#define VALV1_SIGNAL        A4
#define VALV2_SIGNAL        A3
#define VALV3_SIGNAL        A6
#define VALV4_SIGNAL        A5

#define PUMP_ON_SIGNAL       2
#define PUMP_OFF_SIGNAL      3

#define expinMode(a,b)        PCF8574.pinMode(a,b)
#define exdigitalWrite(a,b)   PCF8574.digitalWrite(a,b)
#define exdigitalRead(a)      PCF8574.digitalRead(a)


#define VALV1_FB_SIGNAL     ((uint8_t) 7)  // I/O Expansion chip P7
#define VALV3_FB_SIGNAL     ((uint8_t) 6)  // I/O Expansion chip P7
#define VALV2_FB_SIGNAL     ((uint8_t) 5)  // I/O Expansion chip P7
#define VALV4_FB_SIGNAL     ((uint8_t) 4)  // I/O Expansion chip P7
#define VALV4_CLR_SIGNAL    ((uint8_t) 3)  // I/O Expansion chip P7
#define VALV3_CLR_SIGNAL    ((uint8_t) 2)  // I/O Expansion chip P7
#define VALV2_CLR_SIGNAL    ((uint8_t) 1)  // I/O Expansion chip P7
#define VALV1_CLR_SIGNAL    ((uint8_t) 0)  // I/O Expansion chip P7

#define VALV1_STATE()   exdigitalRead(VALV1_FB_SIGNAL)
#define VALV2_STATE()   exdigitalRead(VALV2_FB_SIGNAL)
#define VALV3_STATE()   exdigitalRead(VALV3_FB_SIGNAL)
#define VALV4_STATE()   exdigitalRead(VALV4_FB_SIGNAL)

#define VALV1_CLR_ON()  exdigitalWrite(VALV1_CLR_SIGNAL, HIGH)
#define VALV2_CLR_ON()  exdigitalWrite(VALV2_CLR_SIGNAL, HIGH)
#define VALV3_CLR_ON()  exdigitalWrite(VALV3_CLR_SIGNAL, HIGH)
#define VALV4_CLR_ON()  exdigitalWrite(VALV4_CLR_SIGNAL, HIGH)

#define VALV1_CLR_OFF()  exdigitalWrite(VALV1_CLR_SIGNAL, LOW)
#define VALV2_CLR_OFF()  exdigitalWrite(VALV2_CLR_SIGNAL, LOW)
#define VALV3_CLR_OFF()  exdigitalWrite(VALV3_CLR_SIGNAL, LOW)
#define VALV4_CLR_OFF()  exdigitalWrite(VALV4_CLR_SIGNAL, LOW)

#define VALV1_ON()    digitalWrite(VALV1_SIGNAL, HIGH)
#define VALV2_ON()    digitalWrite(VALV2_SIGNAL, HIGH)
#define VALV3_ON()    digitalWrite(VALV3_SIGNAL, HIGH)
#define VALV4_ON()    digitalWrite(VALV4_SIGNAL, HIGH)

#define VALV1_OFF()   digitalWrite(VALV1_SIGNAL, LOW)
#define VALV2_OFF()   digitalWrite(VALV2_SIGNAL, LOW)
#define VALV3_OFF()   digitalWrite(VALV3_SIGNAL, LOW)
#define VALV4_OFF()   digitalWrite(VALV4_SIGNAL, LOW)


#define TURN_ON_LED()       digitalWrite(LED_SIGNAL,HIGH)
#define TURN_OFF_LED()      digitalWrite(LED_SIGNAL,LOW)
/*=============================*/

/*====Periodic tasks prototypes====*/
void flowCalcTask(void);
/*===================================*/

/*=====Debug mode=====*/
bool DEBUG_MODE = true;
/*====================*/

/*=====Timer variables=====*/
unsigned long previousMillis;
unsigned long timeInterval;
unsigned long sequence = 0;
int debugMode = 1;
/*=========================*/


/*=====Communication variables=====*/
char input_message_size = 23;
char output_message_size = 41;
char Buffout[80], Buffin[80];
/*=================================*/

/*===========Flow variables===========*/
volatile unsigned long flowCounter1 = 0;
volatile unsigned long flowCounter2 = 0;
volatile unsigned long flowCounter3 = 0;
volatile unsigned long flowCounter4 = 0;
volatile unsigned long flowCounter5 = 0;
volatile float discharge1 = 0;
volatile float discharge2 = 0;
volatile float discharge3 = 0;
volatile float discharge4 = 0;
volatile float discharge5 = 0;
unsigned const long flowInterval = 60000;
/*====================================*/

/*===========Pump variables===========*/
unsigned const long pumpInterval = 1000;
/*====================================*/

/*=============I/O Expansion chip===============*/
#define PCF8574_ADDR1  ((uint8_t) 0x20)    // default PCF8574 I2C address
#define PCF8574_ADDR2  ((uint8_t) 0x21)    // alternative PCF8574 I2C address

jm_PCF8574 PCF8574;
/*==============================================*/
/*=====Re-usable class for periodic timed tasks=====*/

/*==================================================*/

/*=============Periodic tasks declaration==============*/
PeriodicTask flowTask(flowInterval, &FLOW_TASK);
PeriodicTask pumpTask(pumpInterval, &PUMP_TASK);
/*=====================================================*/
//---------------------------------------------------------
// Create the new UART instance for Xbee communication assigning it to pin 8(TX) and 9(RX)
//---------------------------------------------------------
//Uart SerialXbee(&sercom1, 8, 9, SERCOM_RX_PAD_1, UART_TX_PAD_0);


void setup() {

  Serial.begin(115200);   // Configure USB serial as serial monitor
  //while (!Serial);
  Serial1.begin(9600);  // Configure Pins 13(RX),14(TX) for Bluetooth communication
  while (!Serial1);

  //SerialXbee.begin(9600); // Configure Pins 8(TX),9(RX) for Xbee communication
  //while (!SerialXbee);
  //pinPeripheral(8, PIO_SERCOM);
  //pinPeripheral(9, PIO_SERCOM);

  //PCF8574 Initialization
  Wire.begin();
  if (!PCF8574.begin(PCF8574_ADDR1)) {
    Serial.println("Failed to initialize I/O expansion chip. Trying alternative address...");
    if (!PCF8574.begin(PCF8574_ADDR2)) {
      Serial.println("Failed to initialize I/O expansion chip.");
    }
    else {
      Serial.println("I/O expansion chip init successfull with alternative address.");
    }
  }
  else {
    Serial.println("I/O expansion chip init successfull with default address.");
  }


  initPinConfig();
  initISR();

  //Initial state of valves and pump
  clearValves();
  PUMP_OFF();


  delay(1000);
  initialSequence();
  Serial.println("Initialization completed!!!");
}

void loop() {

  flowTask.RunHandler();
  pumpTask.RunHandler();
  //if(SerialXbee.available()){
  if(Serial1.available()){
    xbeeHandler();
  }
  serialCommandHandler();
}

//---------------------------------------------------------
// Initial sequence to identify actuator type
//---------------------------------------------------------
void initialSequence() {
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
// Flow sensor ISRs
//---------------------------------------------------------
//FLOW SENSOR 1 ISR
void flowInterrupt1() {
  if (DEBUG_MODE) {
    TURN_ON_LED();
    delayMicroseconds(2000); //delay() and micros() doesn't work inside ISR!
  }

  flowCounter1++;

  if (DEBUG_MODE) {
    TURN_OFF_LED();
  }
}

//FLOW SENSOR 2 ISR
void flowInterrupt2() {
  if (DEBUG_MODE) {
    TURN_ON_LED();
    delayMicroseconds(2000); //delay() and micros() doesn't work inside ISR!
  }

  flowCounter2++;

  if (DEBUG_MODE) {
    TURN_OFF_LED();
  }
}

//FLOW SENSOR 3 ISR
void flowInterrupt3() {
  if (DEBUG_MODE) {
    TURN_ON_LED();
    delayMicroseconds(2000); //delay() and micros() doesn't work inside ISR!
  }

  flowCounter3++;

  if (DEBUG_MODE) {
    TURN_OFF_LED();
  }
}

//FLOW SENSOR 4 ISR
void flowInterrupt4() {
  if (DEBUG_MODE) {
    TURN_ON_LED();
    delayMicroseconds(2000); //delay() and micros() doesn't work inside ISR!
  }

  flowCounter4++;

  if (DEBUG_MODE) {
    TURN_OFF_LED();
  }
}

//FLOW SENSOR 5 ISR
void flowInterrupt5() {
  if (DEBUG_MODE) {
    TURN_ON_LED();
    delayMicroseconds(2000); //delay() and micros() doesn't work inside ISR!
  }

  flowCounter5++;

  if (DEBUG_MODE) {
    TURN_OFF_LED();
  }
}

void initPinConfig() {

  pinMode(VALV1_SIGNAL, OUTPUT);
  pinMode(VALV2_SIGNAL, OUTPUT);
  pinMode(VALV3_SIGNAL, OUTPUT);
  pinMode(VALV4_SIGNAL, OUTPUT);

  pinMode(PUMP_ON_SIGNAL, OUTPUT);
  pinMode(PUMP_OFF_SIGNAL, OUTPUT);

  expinMode(VALV1_CLR_SIGNAL, OUTPUT);
  expinMode(VALV2_CLR_SIGNAL, OUTPUT);
  expinMode(VALV3_CLR_SIGNAL, OUTPUT);
  expinMode(VALV4_CLR_SIGNAL, OUTPUT);

  pinMode(LED_SIGNAL, OUTPUT);

}

void initISR() {

  attachInterrupt(digitalPinToInterrupt(FSEN1_SIGNAL), flowInterrupt1, RISING);
  attachInterrupt(digitalPinToInterrupt(FSEN2_SIGNAL), flowInterrupt2, RISING);
  attachInterrupt(digitalPinToInterrupt(FSEN3_SIGNAL), flowInterrupt3, RISING);
  attachInterrupt(digitalPinToInterrupt(FSEN4_SIGNAL), flowInterrupt4, RISING);

#if defined(FSEN5_ENABLED)
  attachInterrupt(digitalPinToInterrupt(FSEN5_SIGNAL), flowInterrupt5, RISING);
#endif

}

//---------------------------------------------------------
// Clear all valves state
//---------------------------------------------------------
void clearValves() {

  VALV1_OFF();
  VALV2_OFF();
  VALV3_OFF();
  VALV4_OFF();

  //Clear state of all button controller chips
  VALV1_CLR_ON();
  VALV2_CLR_ON();
  VALV3_CLR_ON();
  VALV4_CLR_ON();
  delay(10);
  VALV1_CLR_OFF();
  VALV2_CLR_OFF();
  VALV3_CLR_OFF();
  VALV4_CLR_OFF();
}

//---------------------------------------------------------
// Handler for debug serial commands
//---------------------------------------------------------
void serialCommandHandler() {


  if (Serial.available()) {
    char serialBuffer[100];
    int i = 0;

    while (Serial.available() > 0) {
      serialBuffer[i] = Serial.read();
      i++;
    }
    serialBuffer[i] = 0;

    if (strstr(serialBuffer, "CMD+READVALVESTATES")) {
      int v1, v2, v3, v4;
      char result[50];
      v1 = VALV1_STATE();
      v2 = VALV2_STATE();
      v3 = VALV3_STATE();
      v4 = VALV4_STATE();

      sprintf(result, "CMD_RESP: ( %d, %d, %d, %d)\n", v1, v2, v3, v4);
      Serial.println(result);
    }
    else if (strstr(serialBuffer, "CMD+VALVE1_ON")) {
      VALV1_ON();
      Serial.println("CMD_RESP:\tOK\n");
    }
    else if (strstr(serialBuffer, "CMD+VALVE2_ON")) {
      VALV2_ON();
      Serial.println("CMD_RESP:\tOK\n");
    }
    else if (strstr(serialBuffer, "CMD+VALVE3_ON")) {
      VALV3_ON();
      Serial.println("CMD_RESP:\tOK\n");
    }
    else if (strstr(serialBuffer, "CMD+VALVE4_ON")) {
      VALV4_ON();
      Serial.println("CMD_RESP:\tOK\n");
    }
    else if (strstr(serialBuffer, "CMD+VALVE1_OFF")) {
      VALV1_OFF();
      Serial.println("CMD_RESP:\tOK\n");
    }
    else if (strstr(serialBuffer, "CMD+VALVE2_OFF")) {
      VALV2_OFF();
      Serial.println("CMD_RESP:\tOK\n");
    }
    else if (strstr(serialBuffer, "CMD+VALVE3_OFF")) {
      VALV3_OFF();
      Serial.println("CMD_RESP:\tOK\n");
    }
    else if (strstr(serialBuffer, "CMD+VALVE4_OFF")) {
      VALV4_OFF();
      Serial.println("CMD_RESP:\tOK\n");
    }
    else {
      Serial.println("COMMAND ERROR\n");
    }
  }
}

//---------------------------------------------------------
// Calculate water consumption from pulses
//---------------------------------------------------------
float flowCalc(unsigned long pulses) {

  float discharge;

  if (pulses > 10) {
    discharge = (0.0407 * pulses) + 2.7478; // Linear approximation formula
  }
  else {
    discharge = 0;
  }
  return discharge;
}

//---------------------------------------------------------
// Flow calculation task
//---------------------------------------------------------
void FLOW_TASK(void) {
  discharge1 = flowCalc(flowCounter1);
  discharge2 = flowCalc(flowCounter2);
  discharge3 = flowCalc(flowCounter3);
  discharge4 = flowCalc(flowCounter4);
  discharge5 = flowCalc(flowCounter5);
  flowCounter1 = 0;
  flowCounter2 = 0;
  flowCounter3 = 0;
  flowCounter4 = 0;
  flowCounter5 = 0;

  if (DEBUG_MODE) {
    Serial.println("/*======FLOW TASK======*/");
    Serial.print("Discharge_1\t=\t");
    Serial.println(discharge1);
    Serial.print("Discharge_2\t=\t");
    Serial.println(discharge2);
    Serial.print("Discharge_3\t=\t");
    Serial.println(discharge3);
    Serial.print("Discharge_4\t=\t");
    Serial.println(discharge4);
    Serial.print("Discharge_5\t=\t");
    Serial.println(discharge5);
    Serial.println("/*======================*/");
  }
}

//---------------------------------------------------------
// PUMP Functions
//---------------------------------------------------------
void PUMP_ON() { //TODO: This is a blocking function (because of delay()), it is better to create an async function.
  digitalWrite(PUMP_OFF_SIGNAL, LOW); //Ensure PUMP_OFF relay is off;
  for (int i = 0; i < 3; i++) {
    digitalWrite(PUMP_ON_SIGNAL, HIGH);
    delay(500);
    digitalWrite(PUMP_ON_SIGNAL, LOW);
    delay(500);
  }
}

void PUMP_OFF() { //TODO: This is a blocking function (because of delay()), it is better to create an async function.
  digitalWrite(PUMP_ON_SIGNAL, LOW); //Ensure PUMP_ON relay is off;
  for (int i = 0; i < 3; i++) {
    digitalWrite(PUMP_OFF_SIGNAL, HIGH);
    delay(500);
    digitalWrite(PUMP_OFF_SIGNAL, LOW);
    delay(500);
  }
}

//---------------------------------------------------------
// PUMP ON/OFF monitor task
//---------------------------------------------------------
void PUMP_TASK(void) {
  static int prevValveStates = 0;
  int currentValveStates = VALV1_STATE() || VALV2_STATE() || VALV3_STATE() || VALV4_STATE();

  if (currentValveStates > prevValveStates) {
    if (DEBUG_MODE) {
      Serial.println("\n/*======PUMP TASK======*/\n");
      Serial.println("Turning ON PUMP...");
      Serial.println("\n/*======================*/\n");
    }
    PUMP_ON();
  }
  else if (currentValveStates < prevValveStates) {
    if (DEBUG_MODE) {
      Serial.println("\n/*======PUMP TASK======*/\n");
      Serial.println("Turning OFF PUMP...");
      Serial.println("\n/*======================*/\n");
    }
    PUMP_OFF();
  }

  prevValveStates = currentValveStates;
}

//---------------------------------------------------------
// Process Xbee message and send a response
//---------------------------------------------------------
void xbeeHandler() {
  
  int sum = 0;

    Serial.println("XBEE MESSAGE RECEIVED");
    //while(!SerialXbee.available());   
    //SerialXbee.readBytes(Buffin,input_message_size);
    while(!Serial1.available());   
    Serial1.readBytes(Buffin,input_message_size);
    if(Buffin[0] == 0x7E && Buffin[15] == 'A') {

      //Update VALVE_1 status
      if (Buffin[16] == '1') {
        Serial.println("Turn Valve 1 ON");
        VALV1_ON();
      }
      else if (Buffin[16] == '0') {
        Serial.println("Turn Valve 1 OFF");
        VALV1_OFF();
      }

      //Update VALVE_2 status
      if (Buffin[17] == '1') {
        Serial.println("Turn Valve 2 ON");
        VALV2_ON();
      }
      else if (Buffin[17] == '0') {
        Serial.println("Turn Valve 2 OFF");
        VALV2_OFF();
      }

      //Update VALVE_3 status
      if (Buffin[18] == '1') {
        Serial.println("Turn Valve 3 ON");
        VALV3_ON();
      }
      else if (Buffin[18] == '0') {
        Serial.println("Turn Valve 3 OFF");
        VALV3_OFF();
      }

      //Update VALVE_4 status
      if (Buffin[19] == '1') {
        Serial.println("Turn Valve 4 ON");
        VALV4_ON();
      }
      else if (Buffin[19] == '0') {
        Serial.println("Turn Valve 4 OFF");
        VALV4_OFF();
      }

      Buffout[0] = 0x7E; //API mode start delimeter
      Buffout[1] = 0x00; //Data length MSB
      Buffout[2] = output_message_size - 4; //Data length LSB
      Buffout[3] = 0x10; //API ID TX Request 16 bit address
      Buffout[4] = 0x00; //Frame ID, if equals to 0x00 disable response frame
      Buffout[5] = Buffin[4];
      Buffout[6] = Buffin[5];
      Buffout[7] = Buffin[6];
      Buffout[8] = Buffin[7];
      Buffout[9] = Buffin[8];
      Buffout[10] = Buffin[9];
      Buffout[11] = Buffin[10];
      Buffout[12] = Buffin[11]; //Destination Address LSB
      Buffout[13] = 0xFF;
      Buffout[14] = 0xFE;
      Buffout[15] = 0x00;
      Buffout[16] = 0x00;
      Buffout[17] = 'A';
      Buffout[18] = VALV1_STATE();
      Buffout[19] = VALV2_STATE();
      Buffout[20] = VALV3_STATE();
      Buffout[21] = VALV4_STATE();
      Buffout[22] = highByte((unsigned int)(discharge1 * 100)); //Water Flow liters/minute
      Buffout[23] = lowByte((unsigned int)(discharge1 * 100)); //Water Flow liters/minute
      Buffout[24] = highByte((unsigned int)(discharge2 * 100)); //Water Flow liters/minute
      Buffout[25] = lowByte((unsigned int)(discharge2 * 100)); //Water Flow liters/minute
      Buffout[26] = highByte((unsigned int)(discharge3 * 100)); //Water Flow liters/minute
      Buffout[27] = lowByte((unsigned int)(discharge3 * 100)); //Water Flow liters/minute
      Buffout[28] = highByte((unsigned int)(discharge4 * 100)); //Water Flow liters/minute
      Buffout[29] = lowByte((unsigned int)(discharge4 * 100)); //Water Flow liters/minute
      Buffout[30] = highByte((unsigned int)(discharge5 * 100)); //Water Flow liters/minute
      Buffout[31] = lowByte((unsigned int)(discharge5 * 100)); //Water Flow liters/minute
      Buffout[32] = 0x00;
      Buffout[33] = 0x00;
      Buffout[34] = 0x00;
      Buffout[35] = 0x00;
      Buffout[36] = 0x00;
      Buffout[37] = 0x00;
      Buffout[38] = 0x00;
      Buffout[39] = 0x0A; //End of package
      for (int i = 3; i < output_message_size - 1; i++) {
        sum += Buffout[i]; //Checksum
      }
      Buffout[40] = 0xFF - lowByte(sum);
      sum = 0;
      //SerialXbee.write(Buffout, output_message_size);
      Serial1.write(Buffout, output_message_size);
    }
    if (DEBUG_MODE) {
      Serial.println("XBEE MESSAGE TRANSMITED");
    }

}


//---------------------------------------------------------
// Attach the interrupt handler to the SERCOM
//---------------------------------------------------------
//void SERCOM1_Handler()
//{
//  SerialXbee.IrqHandler();
//}
