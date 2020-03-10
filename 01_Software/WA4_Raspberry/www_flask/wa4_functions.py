import csv
import array
import datetime

#--------------------------------------------------------------------------------
# Read data file
#--------------------------------------------------------------------------------
def read_data_file(wa4_area):
    
    print('Reading Data File '+wa4_area.data_file_name)
    with open(wa4_area.data_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            wa4_area.timestamp=row[0]
            wa4_area.smC=float(row[1])
            wa4_area.sm1=float(row[2])
            wa4_area.sm2=float(row[3])
            wa4_area.sm3=float(row[4])
            wa4_area.valve_status=int(row[5])
            wa4_area.valve_flow=float(row[6])
            wa4_area.w_radiation=float(row[7])
            wa4_area.w_temperature=float(row[8])
            wa4_area.w_humidity=float(row[9])
            wa4_area.w_wind=float(row[10])
            wa4_area.w_eto=float(row[11])
            wa4_area.ndvi_alpha=float(row[12])
            wa4_area.ndvi_value=float(row[13])
            wa4_area.ndvi_red=float(row[14])
            wa4_area.ndvi_nir=float(row[15])
            wa4_area.error_code=int(row[16])
        csvfile.close()
    return


#--------------------------------------------------------------------------------
# Read config files
#--------------------------------------------------------------------------------
def read_config_files(wa4_config):
    
    print('Reading Config File '+wa4_config.config_mode_file_name)
    with open(wa4_config.config_mode_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            if row[0]== 'operation_mode':
                wa4_config.operation_mode=int(row[1])
        csvfile.close()

    print('Reading Config File '+wa4_config.config_manual_file_name)
    with open(wa4_config.config_manual_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            if row[0]== 'manual_control':
                wa4_config.manual_control=int(row[1])
        csvfile.close()

    print('Reading Config File '+wa4_config.config_limits_file_name)
    with open(wa4_config.config_limits_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            if row[0]== 'irrigation_high_limit':
                wa4_config.high_limit=float(row[1])
            if row[0]== 'irrigation_low_limit':
                wa4_config.low_limit=float(row[1])
        csvfile.close()

    print('Reading Config File '+wa4_config.config_sched_file_name)
    with open(wa4_config.config_sched_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            for i in range (7):
                if row[0]==str(int('0')+i):
                    wa4_config.day_start_h[i]=int(row[1])
                    wa4_config.day_start_m[i]=int(row[2])
                    wa4_config.day_end_h[i]=int(row[3])
                    wa4_config.day_end_m[i]=int(row[4])
                    wa4_config.day_of_week[i]=row[5]                       
        csvfile.close()

    return


#--------------------------------------------------------------------------------
# Write config files
#--------------------------------------------------------------------------------
def write_config_files(wa4_config):
    
    print('Writing Config File '+wa4_config.config_mode_file_name)
    with open(wa4_config.config_mode_file_name,"w+") as csvfile:
        writeCSV=csv.writer(csvfile, delimiter=',')
        writeCSV.writerow(['operation_mode',str('{:d}'.format(wa4_config.operation_mode))])
        csvfile.close()

    if wa4_config.operation_mode==0:
        print('Writing Config File '+wa4_config.config_manual_file_name)
        with open(wa4_config.config_manual_file_name,"w+") as csvfile:
            writeCSV=csv.writer(csvfile, delimiter=',')
            writeCSV.writerow(['manual_control',str('{:d}'.format(wa4_config.manual_control))])
            csvfile.close()

    if wa4_config.operation_mode==1:
        print('Writing Config File '+wa4_config.config_sched_file_name)
        with open(wa4_config.config_sched_file_name,"w+") as csvfile:
            writeCSV=csv.writer(csvfile, delimiter=',')
            for i in range (7):
                writeCSV.writerow([str('{:d}'.format(i)),
                                   str('{:02d}'.format(wa4_config.day_start_h[i])),
                                   str('{:02d}'.format(wa4_config.day_start_m[i])),
                                   str('{:02d}'.format(wa4_config.day_end_h[i])),
                                   str('{:02d}'.format(wa4_config.day_end_m[i])),
                                   wa4_config.day_of_week[i]
                                   ])
            csvfile.close()
            
    if wa4_config.operation_mode==2:
        print('Writing Config File '+wa4_config.config_limits_file_name)
        with open(wa4_config.config_limits_file_name,"w+") as csvfile:
            writeCSV=csv.writer(csvfile, delimiter=',')
            writeCSV.writerow(['irrigation_high_limit',str('{:5.2f}'.format(wa4_config.high_limit))])
            writeCSV.writerow(['irrigation_low_limit',str('{:5.2f}'.format(wa4_config.low_limit))])
            csvfile.close()

    return

#--------------------------------------------------------------------------------
# Get Data from All Areas
#--------------------------------------------------------------------------------
def getDataAllAreas(area1,area2,area3,area4):
    templateData = {
        'smC1'      : area1.smC,
        'smC2'      : area2.smC,
        'smC3'      : area3.smC,
        'smC4'      : area4.smC,
        'valve1'    : area1.valve_status,
        'valve2'    : area2.valve_status,
        'valve3'    : area3.valve_status,
        'valve4'    : area4.valve_status,
        'flow1'     : area1.valve_flow,
        'flow2'     : area2.valve_flow,
        'flow3'     : area3.valve_flow,
        'flow4'     : area4.valve_flow,
    }
    return templateData
    
#--------------------------------------------------------------------------------
# Get Data from One Area
#--------------------------------------------------------------------------------
def getDataOneArea(area):
    templateData = {
        'timestamp' : area.timestamp,
        'smC'       : area.smC,
        'sm1'       : area.sm1,
        'sm2'       : area.sm2,
        'sm3'       : area.sm3,
        'valve'     : area.valve_status,
        'flow'      : area.valve_flow,
        'w_eto'     : area.w_eto,
    }
    return templateData

#--------------------------------------------------------------------------------
# Get Config from One Area
#--------------------------------------------------------------------------------
def getConfigOneArea(conf):
    templateData = {
        'mode'      : conf.operation_mode,
        'm_control' : conf.manual_control,
        'h_limit'   : conf.high_limit,
        'l_limit'   : conf.low_limit,
        'start_h'   : conf.day_start_h,
        'start_m'   : conf.day_start_m,
        'end_h'     : conf.day_end_h,
        'end_m'     : conf.day_end_m,
        'day'       : conf.day_of_week,
    }
    return templateData

#--------------------------------------------------------------------------------
# Get Weather Data
#--------------------------------------------------------------------------------
def getWeatherData(area):
    templateData = {
        'timestamp' : area.timestamp,
        'w_eto'     : area.w_eto,
        'w_tem'     : area.w_temperature,
        'w_rad'     : area.w_radiation,
        'w_hum'     : area.w_humidity,
        'w_win'     : area.w_wind,
    }
    return templateData
