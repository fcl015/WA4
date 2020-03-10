#!/usr/bin/env python

import wa_xbee_comm
import wa_config_data
import wa_control
import wa_iot_comm


#---------------------------------------------------------------
# Define data strcutures
#---------------------------------------------------------------
class irrigation_area:
        def __init__(self, id=0):
                self.area_id=id
                self.total_areas=4
                self.operation_mode='none'   #Options: none,manual,sched,auto
                self.config_file='../config/config_area'+str(id)+'.csv'
                self.sched_file='../config/sched_area'+str(id)+'.csv'
                self.manual_control_file='../config/manual_control_area'+str(id)+'.csv'
                self.operation_mode_file='../config/operation_mode_area'+str(id)+'.csv'
                self.limits_file='../config/limits_area'+str(id)+'.csv'
                self.log_file='../www/html/wa4_log/log_area'+str(id)+'.csv'
                self.data_file='../data/data_area'+str(id)+'.csv'
                self.verbose=0

                Q=0.0
                R=0.0
                p1_ant=1.0
                smKalman_ant=0.0
                smC=0.0
                sm1=0.0
                sm2=0.0
                sm3=0.0
                sm_sensors=0
                sm_actuators=0
                sensor_mac_address=bytearray()
                actuator_mac_address=bytearray()
                weather_mac_adress=bytearray()
                ndvi_mac_adress=bytearray()
                valve_status=0
                valve_flow=0.0
                altitude=1440
                w_radiation=0.0
                w_temperature=0.0
                w_humidity=0.0
                w_wind=0.0
                w_eto=0.0
                ndvi_alpha=0.0
                ndvi_value=0.0
                ndvi_red=0.0
                ndvi_nir=0.0
                error_code=0
                comm_status=0
                
                

#---------------------------------------------------------------
# Main program
#---------------------------------------------------------------
area3=irrigation_area(3)
print("IRRIGATION AREA 3")

if area3.verbose:
        print("(1) Read configuration files")
wa_config_data.read_config_file(area3.config_file,area3)
wa_config_data.read_operation_mode_file(area3.operation_mode_file,area3)
if area3.verbose:
        print("    OPERATION MODE: "+area3.operation_mode)

if area3.verbose:
        print("(2) Configure serial port")
ser = wa_xbee_comm.config_xbee_comm('/dev/ttyS0',9600)

if area3.verbose:
        print("(3) Transmit message to weather node")
for i in range(3): 
        wa_xbee_comm.transmit_weather_message(ser,area3,'WEATHER')
        wa_xbee_comm.receive_weather_message(ser,area3,'WEATHER')        
        if area3.comm_status:
                break
        else:
                print("    Communication retry....");

if area3.verbose:
        print("(4) Transmit message to sensor node")
for i in range(3): 
        wa_xbee_comm.transmit_sensor_message(ser,area3,'S1')
        wa_xbee_comm.receive_sensor_message(ser,area3,'S1')
        if area3.comm_status:
                break
        else:
                print("    Communication retry....");

if area3.comm_status:

        if area3.verbose:
                print("(5) Calculate control action")
        wa_control.data_fusion(area3)
        wa_control.control_algorithm(area3)

        if area3.verbose:
                print("(6) Transmit control action to actuator")
        for i in range(3): 
                wa_xbee_comm.transmit_actuator_message(ser,area3,'ACTUATOR')
                wa_xbee_comm.receive_actuator_message(ser,area3,'ACTUATOR')
                if area3.comm_status:
                        break
                else:
                        print("    Communication retry....");

else:
        if area3.verbose:
                print("(5) No control action due SM data not available")
                print("(6) No action transmit to actuator due SM data not available")
        
        
if area3.verbose:
        print("(7)Update configuration file")
wa_config_data.update_config_file(area3.config_file,area3)

if area3.verbose:
        print("(8)Update data file")
wa_config_data.update_data_file(area3.data_file,area3)

if area3.verbose:
        print("(9)Update log file")
wa_config_data.update_log_file(area3.log_file,area3)
	
if area3.verbose:
        print("(10)Upload Data to ThingSpeak")
wa_iot_comm.update_thingspeak_area3(area3)



