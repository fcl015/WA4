# Import grove pi library
import webiopi
import os
import csv
import array
import datetime

# Define variables
sensors1 = [0 for a in range(17)]
modeValue1 = 0
manualValue1 = 0

sensors2 = [0 for a in range(17)]
modeValue2 = 0
manualValue2 = 0

sensors3 = [0 for a in range(17)]
modeValue3 = 0
manualValue3 = 0

sensors4 = [0 for a in range(17)]
modeValue4 = 0
manualValue4 = 0

def setup():
    # Configure variables
    webiopi.sleep(1)

def loop():
    # Read measurements Area1
    data_file_name="/home/pi/Code/WA4/WA4_Raspberry/data/data_area1.csv"
    with open(data_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            sensors1[0]=row[0]
            sensors1[1]=row[1]
            sensors1[2]=row[2]
            sensors1[3]=row[3]
            sensors1[4]=row[4]
            sensors1[5]=row[5]
            sensors1[6]=row[6]
            sensors1[7]=row[7]
            sensors1[8]=row[8]
            sensors1[9]=row[9]
            sensors1[10]=row[10]
            sensors1[11]=row[11]
            sensors1[12]=row[12]
            sensors1[13]=row[13]
            sensors1[14]=row[14]
            sensors1[15]=row[15]
            sensors1[16]=row[16]
        csvfile.close()

    # Read measurements Area2
    data_file_name="/home/pi/Code/WA4/WA4_Raspberry/data/data_area2.csv"
    with open(data_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            sensors2[0]=row[0]
            sensors2[1]=row[1]
            sensors2[2]=row[2]
            sensors2[3]=row[3]
            sensors2[4]=row[4]
            sensors2[5]=row[5]
            sensors2[6]=row[6]
            sensors2[7]=row[7]
            sensors2[8]=row[8]
            sensors2[9]=row[9]
            sensors2[10]=row[10]
            sensors2[11]=row[11]
            sensors2[12]=row[12]
            sensors2[13]=row[13]
            sensors2[14]=row[14]
            sensors2[15]=row[15]
            sensors2[16]=row[16]
        csvfile.close()

    # Read measurements Area3
    data_file_name="/home/pi/Code/WA4/WA4_Raspberry/data/data_area3.csv"
    with open(data_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            sensors3[0]=row[0]
            sensors3[1]=row[1]
            sensors3[2]=row[2]
            sensors3[3]=row[3]
            sensors3[4]=row[4]
            sensors3[5]=row[5]
            sensors3[6]=row[6]
            sensors3[7]=row[7]
            sensors3[8]=row[8]
            sensors3[9]=row[9]
            sensors3[10]=row[10]
            sensors3[11]=row[11]
            sensors3[12]=row[12]
            sensors3[13]=row[13]
            sensors3[14]=row[14]
            sensors3[15]=row[15]
            sensors3[16]=row[16]
        csvfile.close()

    # Read measurements Area4
    data_file_name="/home/pi/Code/WA4/WA4_Raspberry/data/data_area4.csv"
    with open(data_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            sensors4[0]=row[0]
            sensors4[1]=row[1]
            sensors4[2]=row[2]
            sensors4[3]=row[3]
            sensors4[4]=row[4]
            sensors4[5]=row[5]
            sensors4[6]=row[6]
            sensors4[7]=row[7]
            sensors4[8]=row[8]
            sensors4[9]=row[9]
            sensors4[10]=row[10]
            sensors4[11]=row[11]
            sensors4[12]=row[12]
            sensors4[13]=row[13]
            sensors4[14]=row[14]
            sensors4[15]=row[15]
            sensors4[16]=row[16]
        csvfile.close()

#----------------------------------------------------
# Macros to read sensor data
#----------------------------------------------------
@webiopi.macro
def getSensor1(channel):
    if( int(channel)<17 ):
        return (sensors1[int(channel)])
    else:
        return 'XXXX'

@webiopi.macro
def getSensor2(channel):
    if( int(channel)<17 ):
        return (sensors2[int(channel)])
    else:
        return 'XXXX'

@webiopi.macro
def getSensor3(channel):
    if( int(channel)<17 ):
        return (sensors3[int(channel)])
    else:
        return 'XXXX'

@webiopi.macro
def getSensor4(channel):
    if( int(channel)<17 ):
        return (sensors4[int(channel)])
    else:
        return 'XXXX'

#----------------------------------------------------
# Macros to obtain operation mode
#----------------------------------------------------
@webiopi.macro
def getModeValue1():
    mode_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/operation_mode_area1.csv"
    with open(mode_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            if row[0]=='operation_mode':
                modeValue1=int(row[1])
        csvfile.close()   
    return "%d" % modeValue1

@webiopi.macro
def getModeValue2():
    mode_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/operation_mode_area2.csv"
    with open(mode_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            if row[0]=='operation_mode':
                modeValue2=int(row[1])
        csvfile.close()   
    return "%d" % modeValue2

@webiopi.macro
def getModeValue3():
    mode_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/operation_mode_area3.csv"
    with open(mode_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            if row[0]=='operation_mode':
                modeValue3=int(row[1])
        csvfile.close()   
    return "%d" % modeValue3

@webiopi.macro
def getModeValue4():
    mode_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/operation_mode_area4.csv"
    with open(mode_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            if row[0]=='operation_mode':
                modeValue4=int(row[1])
        csvfile.close()   
    return "%d" % modeValue4

#----------------------------------------------------
# Macros to set operation mode
#----------------------------------------------------
@webiopi.macro
def setModeValue1(value):
    global modeValue1
    mode_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/operation_mode_area1.csv"
    if (int(value) <= 2 and int(value) >= 0 ):
        modeValue1=int(value)
        with open(mode_file_name,"w+") as csvfile:
            writeCSV=csv.writer(csvfile, delimiter=',')
            writeCSV.writerow(["operation_mode",value])
            csvfile.close()
            os.system("/home/pi/Code/WA4/WA4_Raspberry/scripts/start_WA4_Raspberry_Area1.sh")
    return getModeValue1();

@webiopi.macro
def setModeValue2(value):
    global modeValue2
    mode_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/operation_mode_area2.csv"
    if (int(value) <= 2 and int(value) >= 0 ):
        modeValue2=int(value)
        with open(mode_file_name,"w+") as csvfile:
            writeCSV=csv.writer(csvfile, delimiter=',')
            writeCSV.writerow(["operation_mode",value])
            csvfile.close()
            os.system("/home/pi/Code/WA4/WA4_Raspberry/scripts/start_WA4_Raspberry_Area2.sh")
    return getModeValue2();

@webiopi.macro
def setModeValue3(value):
    global modeValue3
    mode_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/operation_mode_area3.csv"
    if (int(value) <= 2 and int(value) >= 0 ):
        modeValue3=int(value)
        with open(mode_file_name,"w+") as csvfile:
            writeCSV=csv.writer(csvfile, delimiter=',')
            writeCSV.writerow(["operation_mode",value])
            csvfile.close()
            os.system("/home/pi/Code/WA4/WA4_Raspberry/scripts/start_WA4_Raspberry_Area3.sh")
    return getModeValue3();

@webiopi.macro
def setModeValue4(value):
    global modeValue4
    mode_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/operation_mode_area4.csv"
    if (int(value) <= 2 and int(value) >= 0 ):
        modeValue4=int(value)
        with open(mode_file_name,"w+") as csvfile:
            writeCSV=csv.writer(csvfile, delimiter=',')
            writeCSV.writerow(["operation_mode",value])
            csvfile.close()
            os.system("/home/pi/Code/WA4/WA4_Raspberry/scripts/start_WA4_Raspberry_Area4.sh")
    return getModeValue4();

#----------------------------------------------------
# Macros to obtain activation value in manual mode
#----------------------------------------------------
@webiopi.macro
def getManualValue1():
    manual_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/manual_control_area1.csv"
    with open(manual_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            if row[0]=='manual_control':
                manualValue1=int(row[1])
        csvfile.close()   
    return "%d" % manualValue1

@webiopi.macro
def getManualValue2():
    manual_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/manual_control_area2.csv"
    with open(manual_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            if row[0]=='manual_control':
                manualValue2=int(row[1])
        csvfile.close()   
    return "%d" % manualValue2

@webiopi.macro
def getManualValue3():
    manual_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/manual_control_area3.csv"
    with open(manual_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            if row[0]=='manual_control':
                manualValue3=int(row[1])
        csvfile.close()   
    return "%d" % manualValue3

@webiopi.macro
def getManualValue4():
    manual_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/manual_control_area4.csv"
    with open(manual_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            if row[0]=='manual_control':
                manualValue4=int(row[1])
        csvfile.close()   
    return "%d" % manualValue4

#----------------------------------------------------
# Macros to set activation value in manual mode
#----------------------------------------------------
@webiopi.macro
def setManualValue1(value):
    global manualValue1
    manual_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/manual_control_area1.csv"
    if (int(value) <= 1 and int(value) >= 0 ):
        manualValue1=int(value)
        with open(manual_file_name,"w+") as csvfile:
            writeCSV=csv.writer(csvfile, delimiter=',')
            writeCSV.writerow(["manual_control",value])
            csvfile.close()
            os.system("/home/pi/Code/WA4/WA4_Raspberry/scripts/start_WA4_Raspberry_Area1.sh")
    return getManualValue1();

@webiopi.macro
def setManualValue2(value):
    global manualValue2
    manual_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/manual_control_area2.csv"
    if (int(value) <= 1 and int(value) >= 0 ):
        manualValue2=int(value)
        with open(manual_file_name,"w+") as csvfile:
            writeCSV=csv.writer(csvfile, delimiter=',')
            writeCSV.writerow(["manual_control",value])
            csvfile.close()
            os.system("/home/pi/Code/WA4/WA4_Raspberry/scripts/start_WA4_Raspberry_Area2.sh")
    return getManualValue2();

@webiopi.macro
def setManualValue3(value):
    global manualValue3
    manual_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/manual_control_area3.csv"
    if (int(value) <= 1 and int(value) >= 0 ):
        manualValue3=int(value)
        with open(manual_file_name,"w+") as csvfile:
            writeCSV=csv.writer(csvfile, delimiter=',')
            writeCSV.writerow(["manual_control",value])
            csvfile.close()
            os.system("/home/pi/Code/WA4/WA4_Raspberry/scripts/start_WA4_Raspberry_Area3.sh")
    return getManualValue3();

@webiopi.macro
def setManualValue4(value):
    global manualValue4
    manual_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/manual_control_area4.csv"
    if (int(value) <= 1 and int(value) >= 0 ):
        manualValue4=int(value)
        with open(manual_file_name,"w+") as csvfile:
            writeCSV=csv.writer(csvfile, delimiter=',')
            writeCSV.writerow(["manual_control",value])
            csvfile.close()
            os.system("/home/pi/Code/WA4/WA4_Raspberry/scripts/start_WA4_Raspberry_Area4.sh")
    return getManualValue4();

#----------------------------------------------------
# Macros to obtain schedule values in time mode
#----------------------------------------------------
@webiopi.macro
def getSchedValue1():
    sched_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/sched_area1.csv"
    final_str=""
    with open(sched_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            final_str=final_str+row[0]+","+row[1]+","+row[2]+","+row[3]+","+row[4]+","+row[5]+"\r\n"
        csvfile.close()
    return "%s" % final_str

@webiopi.macro
def getSchedValue2():
    sched_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/sched_area2.csv"
    final_str=""
    with open(sched_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            final_str=final_str+row[0]+","+row[1]+","+row[2]+","+row[3]+","+row[4]+","+row[5]+"\r\n"
        csvfile.close()
    return "%s" % final_str

@webiopi.macro
def getSchedValue3():
    sched_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/sched_area3.csv"
    final_str=""
    with open(sched_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            final_str=final_str+row[0]+","+row[1]+","+row[2]+","+row[3]+","+row[4]+","+row[5]+"\r\n"
        csvfile.close()
    return "%s" % final_str

@webiopi.macro
def getSchedValue4():
    sched_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/sched_area4.csv"
    final_str=""
    with open(sched_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            final_str=final_str+row[0]+","+row[1]+","+row[2]+","+row[3]+","+row[4]+","+row[5]+"\r\n"
        csvfile.close()
    return "%s" % final_str


#----------------------------------------------------
# Macros to obtain irrigation limits in automatic mode
#----------------------------------------------------
@webiopi.macro
def getLimitsValue1():
    limits_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/limits_area1.csv"
    final_str=""
    with open(limits_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            final_str=final_str+row[0]+","+row[1]+"\r\n"
        csvfile.close()
    return "%s" % final_str

@webiopi.macro
def getLimitsValue2():
    limits_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/limits_area2.csv"
    final_str=""
    with open(limits_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            final_str=final_str+row[0]+","+row[1]+"\r\n"
        csvfile.close()
    return "%s" % final_str

@webiopi.macro
def getLimitsValue3():
    limits_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/limits_area3.csv"
    final_str=""
    with open(limits_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            final_str=final_str+row[0]+","+row[1]+"\r\n"
        csvfile.close()
    return "%s" % final_str

@webiopi.macro
def getLimitsValue4():
    limits_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/limits_area4.csv"
    final_str=""
    with open(limits_file_name) as csvfile:
        readCSV=csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            final_str=final_str+row[0]+","+row[1]+"\r\n"
        csvfile.close()
    return "%s" % final_str

#----------------------------------------------------
# Macros to set schedule value in time mode
#----------------------------------------------------
@webiopi.macro
def setSchedValue1(value):
    sched_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/sched_area1.csv"
    value=value.replace("scheduleData=","")
    value=value.replace("%2C",",")
    value=value.replace("%0D","\r")
    value=value.replace("%0A","\n")
    with open(sched_file_name,"w+") as f:
        f.write(value)
        f.close()
        os.system("/home/pi/Code/WA4/WA4_Raspberry/scripts/start_WA4_Raspberry_Area1.sh")
    return getSchedValue1()

@webiopi.macro
def setSchedValue2(value):
    sched_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/sched_area2.csv"
    value=value.replace("scheduleData=","")
    value=value.replace("%2C",",")
    value=value.replace("%0D","\r")
    value=value.replace("%0A","\n")
    with open(sched_file_name,"w+") as f:
        f.write(value)
        f.close()
        os.system("/home/pi/Code/WA4/WA4_Raspberry/scripts/start_WA4_Raspberry_Area2.sh")
    return getSchedValue2()

@webiopi.macro
def setSchedValue3(value):
    sched_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/sched_area3.csv"
    value=value.replace("scheduleData=","")
    value=value.replace("%2C",",")
    value=value.replace("%0D","\r")
    value=value.replace("%0A","\n")
    with open(sched_file_name,"w+") as f:
        f.write(value)
        f.close()
        os.system("/home/pi/Code/WA4/WA4_Raspberry/scripts/start_WA4_Raspberry_Area3.sh")
    return getSchedValue3()

@webiopi.macro
def setSchedValue4(value):
    sched_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/sched_area4.csv"
    value=value.replace("scheduleData=","")
    value=value.replace("%2C",",")
    value=value.replace("%0D","\r")
    value=value.replace("%0A","\n")
    with open(sched_file_name,"w+") as f:
        f.write(value)
        f.close()
        os.system("/home/pi/Code/WA4/WA4_Raspberry/scripts/start_WA4_Raspberry_Area4.sh")
    return getSchedValue4()

#----------------------------------------------------
# Macros to set limits values in automatic mode
#----------------------------------------------------
@webiopi.macro
def setLimitsValue1(value):
    sched_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/limits_area1.csv"
    value=value.replace("limitsData=","")
    value=value.replace("%2C",",")
    value=value.replace("%0D","\r")
    value=value.replace("%0A","\n")
    with open(sched_file_name,"w+") as f:
        f.write(value)
        f.close()
        os.system("/home/pi/Code/WA4/WA4_Raspberry/scripts/start_WA4_Raspberry_Area1.sh")
    return getLimitsValue1()

@webiopi.macro
def setLimitsValue2(value):
    sched_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/limits_area2.csv"
    value=value.replace("limitsData=","")
    value=value.replace("%2C",",")
    value=value.replace("%0D","\r")
    value=value.replace("%0A","\n")
    with open(sched_file_name,"w+") as f:
        f.write(value)
        f.close()
        os.system("/home/pi/Code/WA4/WA4_Raspberry/scripts/start_WA4_Raspberry_Area2.sh")
    return getLimitsValue2()

@webiopi.macro
def setLimitsValue3(value):
    sched_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/limits_area3.csv"
    value=value.replace("limitsData=","")
    value=value.replace("%2C",",")
    value=value.replace("%0D","\r")
    value=value.replace("%0A","\n")
    with open(sched_file_name,"w+") as f:
        f.write(value)
        f.close()
        os.system("/home/pi/Code/WA4/WA4_Raspberry/scripts/start_WA4_Raspberry_Area3.sh")
    return getLimitsValue3()

@webiopi.macro
def setLimitsValue4(value):
    sched_file_name="/home/pi/Code/WA4/WA4_Raspberry/config/limits_area4.csv"
    value=value.replace("limitsData=","")
    value=value.replace("%2C",",")
    value=value.replace("%0D","\r")
    value=value.replace("%0A","\n")
    with open(sched_file_name,"w+") as f:
        f.write(value)
        f.close()
        os.system("/home/pi/Code/WA4/WA4_Raspberry/scripts/start_WA4_Raspberry_Area4.sh")
    return getLimitsValue4()


