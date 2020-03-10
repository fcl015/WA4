#!/usr/bin/env python

import serial
import array
import datetime
import numpy


#---------------------------------------------------------------
# Configure serial port
#---------------------------------------------------------------
def config_xbee_comm(device,baud_rate):

        ser=serial.Serial(
                port=device,
                baudrate=baud_rate,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS,
                timeout=10
        )
        ser.flushInput()
        ser.flushOutput()
        return ser


#---------------------------------------------------------------
# Transmit message to sensor node
#---------------------------------------------------------------
def transmit_sensor_message(ser,data,message):

        output_message_size=24
        buffer_out=bytearray(output_message_size)
        buffer_out[0]=0x7e  	                # Start character
        buffer_out[1]=0x00	                # MSB data size 
        buffer_out[2]=output_message_size-4	# LSB data size
        buffer_out[3]=0x10	                # TX DigiMesh  64 bits
        buffer_out[4]=0x00	                # Frame ID
        buffer_out[5]=data.sensor_mac_address[0]# 64 bits destination address
        buffer_out[6]=data.sensor_mac_address[1]
        buffer_out[7]=data.sensor_mac_address[2]
        buffer_out[8]=data.sensor_mac_address[3]
        buffer_out[9]=data.sensor_mac_address[4]
        buffer_out[10]=data.sensor_mac_address[5]
        buffer_out[11]=data.sensor_mac_address[6]	
        buffer_out[12]=data.sensor_mac_address[7]
        buffer_out[13]=0xFF	                # 16 bits destination address
        buffer_out[14]=0xFE	
        buffer_out[15]=0x00	                # Broadcast radius
        buffer_out[16]=0x00	                # Options
        buffer_out[17]=ord(message[0])	        # 'S' Node Type
        buffer_out[18]=ord(message[1])          # SM equation number
        buffer_out[19]=int(data.ndvi_alpha*10000//256)  # NDVI Alpha Value High
        buffer_out[20]=int(data.ndvi_alpha*10000%256)   # NDVI Alpha Value Low
        buffer_out[21]=0x00	                # Not used
        buffer_out[22]=0x00	                # Not used
        buffer_out[23]=0x00	                # Checksum
        checksum=0;
        for i in range(3,output_message_size-1):
                checksum=checksum+buffer_out[i]
                if checksum > 0xFF:
                        checksum=checksum-0x100
        checksum=0xFF-checksum
        buffer_out[output_message_size-1]=checksum
        # Send message 
        ser.write(buffer_out)
        return

#---------------------------------------------------------------
# Receive message from sensor node
#---------------------------------------------------------------
def receive_sensor_message(ser,data,message):
        
        input_message_size=30
        data_in=bytearray(0);
        data_in=ser.read(input_message_size)
        try:
                extra_data=ser.inWaiting()
                if (data_in[15]!=ord(message[0]) or data_in[10]!=data.sensor_mac_address[6] or data_in[11]!=data.sensor_mac_address[7]):
                        print("    ERROR SENSOR NODE: Invalid Node Identifier or Invalid MAC address");
                        ser.flushInput()
                        data.comm_status=0;
                        return
                if len(data_in)==input_message_size and extra_data==0:
                        # Extract data from received message
                        data.sm1=(data_in[16]*256+data_in[17])/100
                        data.sm2=(data_in[18]*256+data_in[19])/100
                        data.sm3=(data_in[20]*256+data_in[21])/100
                        data.ndvi_value=(numpy.int16(data_in[22]*256+data_in[23]))/10000
                        data.ndvi_red=(numpy.int16(data_in[24]*256+data_in[25]))/10000
                        data.ndvi_nir=(numpy.int16(data_in[26]*256+data_in[27]))/10000
                        # Display data
                        if data.verbose:
                                print("    SM1={0:5.2f}, SM2={1:5.2f}, SM3={2:5.2f}, NDVI={3:6.4f}, RED={4:6.4f}, NIR={5:6.4f}".format(data.sm1,data.sm2,data.sm3,data.ndvi_value,data.ndvi_red,data.ndvi_nir));
                        data.comm_status=1;
                elif extra_data>0:
                        print("    ERROR SENSOR NODE Incorrect Message: {} extra-bytes received".format(extra_data))
                        ser.flushInput()
                        data.comm_status=0;
                elif len(data_in)>0: 
                        print("    ERROR SENSOR NODE Incomplete Message: {} bytes received".format(len(data_in)))
                        ser.flushInput()
                        data.comm_status=0;
                else:        
                        print("    ERROR SENSOR NODE Timeout: No Message received");
                        data.comm_status=0;
                return
        except:
                print("    ERROR SENSOR NODE Timeout: No Message received");
                data.comm_status=0;
                return
                

#---------------------------------------------------------------
# Transmit message to actuator node
#---------------------------------------------------------------
def transmit_actuator_message(ser,data,message):

        v1=str('{0:04b}'.format(data.sm_actuators))[3]
        v2=str('{0:04b}'.format(data.sm_actuators))[2]
        v3=str('{0:04b}'.format(data.sm_actuators))[1]
        v4=str('{0:04b}'.format(data.sm_actuators))[0]

        output_message_size=24
        buffer_out=bytearray(output_message_size)
        buffer_out[0]=0x7e  	                # Start character
        buffer_out[1]=0x00	                # MSB data size 
        buffer_out[2]=output_message_size-4	# LSB data size
        buffer_out[3]=0x10	                # TX DigiMesh  64 bits
        buffer_out[4]=0x00	                # Frame ID
        buffer_out[5]=data.actuator_mac_address[0] # 64 bits destination address
        buffer_out[6]=data.actuator_mac_address[1]
        buffer_out[7]=data.actuator_mac_address[2]
        buffer_out[8]=data.actuator_mac_address[3]
        buffer_out[9]=data.actuator_mac_address[4]
        buffer_out[10]=data.actuator_mac_address[5]
        buffer_out[11]=data.actuator_mac_address[6]	
        buffer_out[12]=data.actuator_mac_address[7]
        buffer_out[13]=0xFF	                # 16 bits destination address
        buffer_out[14]=0xFE	
        buffer_out[15]=0x00	                # Broadcast radius
        buffer_out[16]=0x00	                # Options
        buffer_out[17]=ord(message[0])	        # Data
        if v1=='1':
                buffer_out[18]=data.valve_status+48
        else:
                buffer_out[18]=ord('X')
        if v2=='1':
                buffer_out[19]=data.valve_status+48
        else:
                buffer_out[19]=ord('X')
        if v3=='1':
                buffer_out[20]=data.valve_status+48
        else:
                buffer_out[20]=ord('X')
        if v4=='1':
                buffer_out[21]=data.valve_status+48
        else:
                buffer_out[21]=ord('X')
        buffer_out[22]=0x00                     # End of transmission
        buffer_out[23]=0x00	                # Checksum
        checksum=0;
        for i in range(3,output_message_size-1):
                checksum=checksum+buffer_out[i]
                if checksum > 0xFF:
                        checksum=checksum-0x100
        checksum=0xFF-checksum
        buffer_out[output_message_size-1]=checksum
        # Send message 
        ser.write(buffer_out)
        return


#---------------------------------------------------------------
# Receive message from actuator node
#---------------------------------------------------------------
def receive_actuator_message(ser,data,message):
        
        v1=str('{0:04b}'.format(data.sm_actuators))[3]
        v2=str('{0:04b}'.format(data.sm_actuators))[2]
        v3=str('{0:04b}'.format(data.sm_actuators))[1]
        v4=str('{0:04b}'.format(data.sm_actuators))[0]

        input_message_size=30
        data_in=bytearray(0);
        data_in=ser.read(input_message_size)
        try:
                extra_data=ser.inWaiting()
                if (data_in[15]!=ord(message[0]) or data_in[10]!=data.actuator_mac_address[6] or data_in[11]!=data.actuator_mac_address[7]):
                        print("    ERROR ACTUATOR NODE: Invalid Node Identifier or Invalid MAC address");
                        ser.flushInput()
                        data.comm_status=0;
                        return
                if len(data_in)==input_message_size and extra_data==0:
                        # Extract data from received message
                        data.valve_flow=0.0
                        if v1=='1':
                                data.valve_flow=(data_in[20]*256+data_in[21])/100
                                if data.verbose:
                                        print("    VALVE1_STATUS_SENT={0:d}, VALVE1_STATUS_RECEIVED={1:d}".format(data.valve_status,data_in[16]))
                        if v2=='1':
                                data.valve_flow=(data_in[22]*256+data_in[23])/100
                                if data.verbose:
                                        print("    VALVE2_STATUS_SENT={0:d}, VALVE2_STATUS_RECEIVED={1:d}".format(data.valve_status,data_in[17]))
                        if v3=='1':        
                                data.valve_flow=(data_in[24]*256+data_in[25])/100
                                if data.verbose:
                                        print("    VALVE3_STATUS_SENT={0:d}, VALVE3_STATUS_RECEIVED={1:d}".format(data.valve_status,data_in[18]))
                        if v4=='1':        
                                data.valve_flow=(data_in[26]*256+data_in[27])/100
                                if data.verbose:
                                        print("    VALVE4_STATUS_SENT={0:d}, VALVE4_STATUS_RECEIVED={1:d}".format(data.valve_status,data_in[19]))
                        if data.verbose:
                                print("    FLOW={0:5.2f}".format(data.valve_flow))
                        data.comm_status=1;
                elif extra_data>0:
                        print("    ERROR ACTUATOR NODE Incorrect Message: {} extra-bytes received".format(extra_data))
                        ser.flushInput()
                        data.comm_status=0;
                elif len(data_in)>0: 
                        print("    ERROR ACTUATOR NODE Incomplete Message: {} bytes received".format(len(data_in)))
                        ser.flushInput()
                        data.comm_status=0;
                else:        
                        print("    ERROR ACTUATOR NODE Timeout: No Message received");
                        data.comm_status=0;
                return        
        except:
                print("    ERROR ACTUATOR NODE Timeout: No Message received");
                data.comm_status=0;
                return

#---------------------------------------------------------------
# Transmit message to weather node
#---------------------------------------------------------------
def transmit_weather_message(ser,data,message):

        output_message_size=24
        buffer_out=bytearray(output_message_size)
        buffer_out[0]=0x7e  	                # Start character
        buffer_out[1]=0x00	                # MSB data size 
        buffer_out[2]=output_message_size-4	# LSB data size
        buffer_out[3]=0x10	                # TX DigiMesh  64 bits
        buffer_out[4]=0x00	                # Frame ID
        buffer_out[5]=data.weather_mac_address[0]# 64 bits destination address
        buffer_out[6]=data.weather_mac_address[1]
        buffer_out[7]=data.weather_mac_address[2]
        buffer_out[8]=data.weather_mac_address[3]
        buffer_out[9]=data.weather_mac_address[4]
        buffer_out[10]=data.weather_mac_address[5]
        buffer_out[11]=data.weather_mac_address[6]	
        buffer_out[12]=data.weather_mac_address[7]
        buffer_out[13]=0xFF	                # 16 bits destination address
        buffer_out[14]=0xFE	
        buffer_out[15]=0x00	                # Broadcast radius
        buffer_out[16]=0x00	                # Options
        buffer_out[17]=ord(message[0])	        # Data
                                                # Month 0-Jan, 1-Feb, ... , 11-Dec
        buffer_out[18]=datetime.datetime.today().month-1                      
                                                # Chihuahua's Altitude=1440 => 0x05A0 
        buffer_out[19]=int(data.altitude)//256       # 05 (high byte) 
        buffer_out[20]=int(data.altitude)%256        # A0 (low byte)
        buffer_out[21]=ord('-')                 # Not used
        buffer_out[22]=0x0A                     # End of transmission
        buffer_out[23]=0x00	                # Checksum
        checksum=0;
        for i in range(3,output_message_size-1):
                checksum=checksum+buffer_out[i]
                if checksum > 0xFF:
                        checksum=checksum-0x100
        checksum=0xFF-checksum
        buffer_out[output_message_size-1]=checksum
        # Send message
        ser.write(buffer_out)
        return

#---------------------------------------------------------------
# Receive message from weather node
#---------------------------------------------------------------
def receive_weather_message(ser,data,message):

        input_message_size=30
        data_in=bytearray(0);
        data_in=ser.read(input_message_size)
        try:
                extra_data=ser.inWaiting()
                if (data_in[15]!=ord(message[0]) or data_in[10]!=data.weather_mac_address[6] or data_in[11]!=data.weather_mac_address[7]):
                        print("    ERROR WEATHER NODE: Invalid Node Identifier or Invalid MAC address");
                        ser.flushInput()
                        data.comm_status=0;
                        return
                if len(data_in)==input_message_size and extra_data==0:
                        # Extract data from received message
                        data.w_radiation=(numpy.int16(data_in[16]*256+data_in[17]))
                        data.w_humidity=(numpy.int16(data_in[18]*256+data_in[19]))/100
                        data.w_temperature=(numpy.int16(data_in[20]*256+data_in[21]))/100
                        data.w_wind=(numpy.int16(data_in[22]*256+data_in[23]))/100
                        data.w_eto=(numpy.int16(data_in[24]*256+data_in[25]))/100
                        alpha_temp=(numpy.int16(data_in[26]*256+data_in[27]))/10000
                        data.error_code=data_in[28]
                        if alpha_temp>0.0 and data.error_code==0:
                                data.ndvi_alpha=alpha_temp;
                        # Display data
                        if data.verbose:
                                print("    RAD={0:5.0f}, HUM={1:5.2f}, TEMP={2:5.2f}, WIND={3:5.2f}, ETO={4:5.2f}, ALPHA={5:7.4f}".format(data.w_radiation,data.w_humidity,data.w_temperature,data.w_wind,data.w_eto,alpha_temp));
                        data.comm_status=1;
                elif extra_data>0:
                        print("    ERROR WEATHER NODE Incorrect Message: {} extra-bytes received".format(extra_data))
                        ser.flushInput()
                        data.comm_status=0;
                elif len(data_in)>0: 
                        print("    ERROR WEATHER NODE Incomplete Message: {} bytes received".format(len(data_in)))
                        ser.flushInput()
                        data.comm_status=0;
                else:        
                        print("    ERROR WEATHER NODE Timeout: No Message received");
                        data.comm_status=0;
                return
        except:
                print("    ERROR WEATHER NODE Timeout: No Message received");
                data.comm_status=0;
                return
                

