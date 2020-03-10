#!/usr/bin/env python

import time
import serial
import array

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

dest_mac_address=bytearray(b'\x00\x13\xa2\x00\x40\xa8\x50\x6e')

# Prepare message to transmit
output_message_size=25
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
buffer_out[17]=ord('A')	# Data
buffer_out[18]=ord('0')
buffer_out[19]=ord('0')
buffer_out[20]=ord('0')
buffer_out[21]=ord('0')
buffer_out[22]=0x00	# Not Used
buffer_out[23]=0x00	# Not Used
buffer_out[24]=0x00	# Checksum

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
input_message_size=39
print ("RECEIVE MESSAGE");

data_in=bytearray(0);
data_in=ser.read(input_message_size)
extra_data=ser.inWaiting()
if len(data_in)==input_message_size and extra_data==0:
        print(','.join([hex(i) for i in data_in]))
elif extra_data>0:
        print("Incorrect Message: {} extra-bytes received".format(extra_data))
        ser.flushInput()
elif len(data_in)>0: 
        print("Incomplete Message: {} bytes received".format(len(data_in)))
        ser.flushInput()
else:        
        print("Timeout: No Message received");

