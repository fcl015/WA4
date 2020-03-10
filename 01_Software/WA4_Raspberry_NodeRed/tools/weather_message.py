#!/usr/bin/env python

import time
import serial
import array
import numpy


# Configure arduino serial port
ser=serial.Serial(
	port='/dev/ttyS0',
	baudrate=9600,
	parity=serial.PARITY_NONE,
	stopbits=serial.STOPBITS_ONE,
	bytesize=serial.EIGHTBITS,
	timeout=10
)
ser.flushInput()
ser.flushOutput()

dest_mac_address=bytearray(b'\x00\x13\xa2\x00\x40\xb1\x35\xbc')


# Prepare message to transmit
output_message_size=24
buffer_out=bytearray(output_message_size)
buffer_out[0]=0x7e  	# Start character
buffer_out[1]=0x00	# MSB data size 
buffer_out[2]=output_message_size-4	# LSB data size
buffer_out[3]=0x10	# TX DigiMesh  64 bits
buffer_out[4]=0x00	# Frame ID
buffer_out[5]=dest_mac_address[0]	# 64 bits destination address
buffer_out[6]=dest_mac_address[1]
buffer_out[7]=dest_mac_address[2]
buffer_out[8]=dest_mac_address[3]
buffer_out[9]=dest_mac_address[4]
buffer_out[10]=dest_mac_address[5]
buffer_out[11]=dest_mac_address[6]	
buffer_out[12]=dest_mac_address[7]
buffer_out[13]=0xFF	# 16 bits destination address
buffer_out[14]=0xFE	
buffer_out[15]=0x00	# Broadcast radius
buffer_out[16]=0x00	# Options
buffer_out[17]=ord('W')	# Data
buffer_out[18]=0x05     # Month 5=June 
                        # Chihuahua's Altitude=1440 => 0x05A0 
buffer_out[19]=0x05     # 05 (high byte) 
buffer_out[20]=0xA0     # A0 (low byte)
buffer_out[21]=ord('-') # Not used
buffer_out[22]=0x0A     # End of transmission
buffer_out[23]=0x00	# Checksum

# Calculate checksum
checksum=0;
for i in range(3,output_message_size-1):
	checksum=checksum+buffer_out[i]
	if checksum > 0xFF:
		checksum=checksum-0x100
checksum=0xFF-checksum
buffer_out[output_message_size-1]=checksum

# Send message to remote node
print(time.strftime("%c"))
print("TRANSMIT MESSAGE");
print(','.join([hex(i) for i in buffer_out]))
ser.write(buffer_out)
	
# Receive answer from remote node
input_message_size=30
print ("RECEIVE MESSAGE");

data_in=bytearray(0);
data_in=ser.read(input_message_size)
extra_data=ser.inWaiting()
if len(data_in)==input_message_size and extra_data==0:
        print(','.join([hex(i) for i in data_in]))
        radiation=(numpy.int16(data_in[16]*256+data_in[17]))
        humidity=(numpy.int16(data_in[18]*256+data_in[19]))/100
        temperature=(numpy.int16(data_in[20]*256+data_in[21]))/100
        wind=(numpy.int16(data_in[22]*256+data_in[23]))/100
        eto=(numpy.int16(data_in[24]*256+data_in[25]))/100
              
        print("RAD={0:7.2f}, HUM={1:5.2f}, TEMP={2:5.2f}, WIND={3:5.2f}, ETO={4:5.2f}".format(radiation,humidity,temperature,wind,eto));
elif extra_data>0:
        print("Incorrect Message: {} extra-bytes received".format(extra_data))
        ser.flushInput()
elif len(data_in)>0: 
        print("Incomplete Message: {} bytes received".format(len(data_in)))
        ser.flushInput()
else:        
        print("Timeout: No Message received");

