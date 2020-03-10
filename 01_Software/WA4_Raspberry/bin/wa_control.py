#!/usr/bin/env python

import serial
import array
import datetime
import wa_config_data
import csv

#---------------------------------------------------------------
# Calculate Kalman value
#---------------------------------------------------------------
def data_fusion(data):

        if data.sm_sensors==0b001:
                data.smC=data.sm1
                if data.verbose:
                        print("    Fusion (SM1)={0:5.2f}".format(data.smC))
        elif data.sm_sensors==0b010:
                data.smC=data.sm2
                if data.verbose:
                        print("    Fusion (SM2)={0:5.2f}".format(data.smC))
        elif data.sm_sensors==0b100:
                data.smC=data.sm3
                if data.verbose:
                        print("    Fusion (SM3)={0:5.2f}".format(data.smC))
        elif data.sm_sensors==0b011:
                data.smC=(data.sm1+data.sm2)/2.0
                if data.verbose:
                        print("    Fusion (SM1,SM2)={0:5.2f}".format(data.smC))
        elif data.sm_sensors==0b110:
                data.smC=(data.sm2+data.sm3)/2.0
                if data.verbose:
                        print("    Fusion (SM2,SM3)={0:5.2f}".format(data.smC))
        elif data.sm_sensors==0b101:
                data.smC=(data.sm1+data.sm3)/2.0
                if data.verbose:
                        print("    Fusion (SM1,SM3)={0:5.2f}".format(data.smC))
        elif data.sm_sensors==0b111:
                data.smC=(data.sm1+data.sm2+data.sm3)/3.0
                if data.verbose:
                        print("    Fusion (SM1,SM2,SM3)={0:5.2f}".format(data.smC))
        else:
                data.smC=0;
                print("    ERROR: NO DATA FUSION")

        if data.Q==0.0 or data.R==0.0:
                print("    NO KALMAN FILTER");
        else:        
                calculate_kalman(data)
        return 

#---------------------------------------------------------------
# Calculate Kalman value
#---------------------------------------------------------------
def calculate_kalman(data):

        k=(data.p1_ant+data.Q)/(data.p1_ant+data.Q+data.R)
        data.smC=data.smKalman_ant+k*(data.smC-data.smKalman_ant)
        p1=(data.p1_ant+data.Q)*(1-k)
        data.p1_ant=p1
        data.smKalman_ant=data.smC
        print("    ETo={0:5.2f}".format(data.w_eto));
        print("    SM_fusion={0:5.2f}".format(data.smC));
        return 


#---------------------------------------------------------------
# Verify constraints to activate irrigation
#---------------------------------------------------------------
def verify_constraints(data):

        if data.verbose:
                print('    VERIFYING CONSTRAINTS...')
                print('        Current irrigation area: '+str(data.area_id))

        for other_area in range(1, data.total_areas+1):
                if other_area != data.area_id:
                        file_name='../config/config_area'+str(other_area)+'.csv'
                        if data.verbose:                        
                                print('        Analyzing file: '+file_name)
                        with open(file_name) as csvfile:
                                readCSV=csv.reader(csvfile, delimiter='=')
                                for row in readCSV:
                                        if row[0]== 'valve_status':
                                                other_valve_status=int(row[1])
                                                if data.verbose:
                                                        print('        Valve Status: '+str(other_valve_status))
                                                if other_valve_status==1:
                                                        if data.verbose:
                                                                print('        CONSTRAINT: Irrigation area '+str(other_area)+' is active')
                                                        return False
                                csvfile.close()

        if data.verbose:
                print('        No constraint for irrigation')

        return True

#---------------------------------------------------------------
# Control algorithm
#---------------------------------------------------------------
def control_algorithm(data):

        if data.operation_mode=='auto':

                print('    Control algorithm in AUTO mode')
                if verify_constraints(data):
                        if wa_config_data.read_limits_file(data.limits_file,data.smC,data.valve_status,data.verbose):
                                data.valve_status=1;
                                print("    Valve status ({0:d})".format(data.valve_status));
                        else:
                                data.valve_status=0;
                                print("    Valve status ({0:d})".format(data.valve_status));                        
                        return
                else:
                        print('    No action since there is a constraint')
        
        elif data.operation_mode=='sched':

                print('    Control algorithm in SCHED mode')
                if verify_constraints(data):
                        c_weekday=datetime.datetime.today().weekday()
                        c_hour=datetime.datetime.today().hour
                        c_min=datetime.datetime.today().minute  
                        if wa_config_data.read_schedule_file(data.sched_file,c_weekday,c_hour,c_min):
                                data.valve_status=1;
                                print("    Valve status ({0:d})".format(data.valve_status));
                        else:
                                data.valve_status=0;
                                print("    Valve status ({0:d})".format(data.valve_status));
                        return
                else:
                        print('    No action since there is a constraint')

        elif data.operation_mode=='manual':

                print('    Control algorithm in MANUAL mode')
                if verify_constraints(data):
                        if wa_config_data.read_manual_control_file(data.manual_control_file):
                                data.valve_status=1;
                                print("    Valve status ({0:d})".format(data.valve_status));
                        else:
                                data.valve_status=0;
                                print("    Valve status ({0:d})".format(data.valve_status));
                        return
                else:
                        print('    No action since there is a constraint')

        else:
                print('    Control algorithm NOT-DEFINED mode')
                return
                
                



