import wa4_functions
import json
import os
import subprocess
from flask import Flask, render_template, request, send_file, redirect
app = Flask(__name__)

#---------------------------------------------------------------
# Define data strcutures
#---------------------------------------------------------------
nodered_port="1880"
host_ip="0.0.0.0"

#---------------------------------------------------------------
# Define data strcutures
#---------------------------------------------------------------
class wa4_area:
    def __init__(self, id=0):
        self.area_id=id
        self.data_file_name='../data/data_area'+str(id)+'.csv'
        self.timestamp='Not Defined'
        
        smC=0.0
        sm1=0.0
        sm2=0.0
        sm3=0.0
        valve_status=0
        valve_flow=0.0
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

class wa4_config:
    def __init__(self, id=0):
        self.area_id=id
        self.config_mode_file_name='../config/operation_mode_area'+str(id)+'.csv'
        self.config_manual_file_name='../config/manual_control_area'+str(id)+'.csv'
        self.config_sched_file_name='../config/sched_area'+str(id)+'.csv'
        self.config_limits_file_name='../config/limits_area'+str(id)+'.csv'
        
        operation_mode=0
        manual_control=0
        high_limit=0.0
        low_limit=0.0
        self.day_start_h=[0,0,0,0,0,0,0]
        self.day_start_m=[0,0,0,0,0,0,0]
        self.day_end_h=[0,0,0,0,0,0,0]
        self.day_end_m=[0,0,0,0,0,0,0]
        self.day_of_week=['None','None','None','None','None','None','None']
                
#---------------------------------------------------------------
# Create variable data strcutures
#---------------------------------------------------------------
area1=wa4_area(1)
area2=wa4_area(2)
area3=wa4_area(3)
area4=wa4_area(4)
conf1=wa4_config(1)
conf2=wa4_config(2)
conf3=wa4_config(3)
conf4=wa4_config(4)

#--------------------------------------------------------------------------------
# Menu principal de opciones
#--------------------------------------------------------------------------------
@app.route("/")
def index():
    return render_template('index.html')

#--------------------------------------------------------------------------------
# Menu principal de opciones
#--------------------------------------------------------------------------------
@app.route("/index2/")
def index2():
    return render_template('index2.html')

#--------------------------------------------------------------------------------
# Resumen
#--------------------------------------------------------------------------------
@app.route("/summary_index/")
def summary_index():
    global host_ip, nodered_port
    print("Redirecting to http://"+host_ip+":"+nodered_port+"/ui/#!/0/")
    return redirect("http://"+host_ip+":"+nodered_port+"/ui/#!/0/")


#--------------------------------------------------------------------------------
# Monitoreo
#--------------------------------------------------------------------------------
@app.route("/monitor_index/")
def monitor_index():
    return render_template('monitor_index.html')

#--------------------------------------------------------------------------------
# Monitoreo Area1
#--------------------------------------------------------------------------------
@app.route("/area1/")
def monitor_area1():
    global host_ip, nodered_port
    print("Redirecting to http://"+host_ip+":"+nodered_port+"/ui/#!/1/")
    return redirect("http://"+host_ip+":"+nodered_port+"/ui/#!/1/")

#--------------------------------------------------------------------------------
# Monitoreo Area2
#--------------------------------------------------------------------------------
@app.route("/area2/")
def monitor_area2():
    global host_ip, nodered_port
    print("Redirecting to http://"+host_ip+":"+nodered_port+"/ui/#!/2/")
    return redirect("http://"+host_ip+":"+nodered_port+"/ui/#!/2/")

#--------------------------------------------------------------------------------
# Monitoreo Area3
#--------------------------------------------------------------------------------
@app.route("/area3/")
def monitor_area3():
    global host_ip, nodered_port
    print("Redirecting to http://"+host_ip+":"+nodered_port+"/ui/#!/3/")
    return redirect("http://"+host_ip+":"+nodered_port+"/ui/#!/3/")

#--------------------------------------------------------------------------------
# Monitoreo Area4
#--------------------------------------------------------------------------------
@app.route("/area4/")
def monitor_area4():
    global host_ip, nodered_port
    print("Redirecting to http://"+host_ip+":"+nodered_port+"/ui/#!/4/")
    return redirect("http://"+host_ip+":"+nodered_port+"/ui/#!/4/")

#--------------------------------------------------------------------------------
# Control
#--------------------------------------------------------------------------------
@app.route("/control_index/")
def control_index():
    return render_template('control_index.html')

#--------------------------------------------------------------------------------
# Control Area1
#--------------------------------------------------------------------------------
@app.route("/control_area1/")
def control_area1():
    global conf1

    wa4_functions.read_config_files(conf1)    
    templateData=wa4_functions.getConfigOneArea(conf1)
    return render_template('control_area1.html', **templateData)

#--------------------------------------------------------------------------------
# Control Area1 Operation Mode
#--------------------------------------------------------------------------------
@app.route("/control_area1/<deviceName>")
def control_area1_mode(deviceName):
    global conf1

    if deviceName == 'manual':
        conf1.operation_mode=0
    if deviceName == 'sched':
        conf1.operation_mode=1
    if deviceName == 'auto':
        conf1.operation_mode=2  

    wa4_functions.write_config_files(conf1)      
    templateData=wa4_functions.getConfigOneArea(conf1)
    return render_template('control_area1.html', **templateData)

#--------------------------------------------------------------------------------
# Control Area1 Manual Control
#--------------------------------------------------------------------------------
@app.route("/area1_manual_control/", methods=['POST'])
def area1_manual_control():
    
    global conf1
    conf1.operation_mode = int(request.form['m_mode'])
    conf1.manual_control = int(request.form['control'])
    wa4_functions.write_config_files(conf1)
    os.system('sudo /usr/bin/python3 /home/pi/Code/WA4/WA4_Raspberry/bin/control_loop_area1.py')

    wa4_functions.read_config_files(conf1)    
    templateData=wa4_functions.getConfigOneArea(conf1)
    return render_template('control_area1.html', **templateData)

#--------------------------------------------------------------------------------
# Control Area1 Sched Control
#--------------------------------------------------------------------------------
@app.route("/area1_sched/", methods=['POST'])
def area1_sched():
    
    global conf1
    
    conf1.operation_mode = int(request.form['s_mode'])
    for i in range (7):
        conf1.day_start_h[i] = int(request.form['start_h'+str(i)])
        conf1.day_start_m[i] = int(request.form['start_m'+str(i)])
        conf1.day_end_h[i] = int(request.form['end_h'+str(i)])
        conf1.day_end_m[i] = int(request.form['end_m'+str(i)])
        conf1.day_of_week[i] = request.form['day'+str(i)]
    wa4_functions.write_config_files(conf1)      
    os.system('sudo /usr/bin/python3 /home/pi/Code/WA4/WA4_Raspberry/bin/control_loop_area1.py')

    wa4_functions.read_config_files(conf1)    
    templateData=wa4_functions.getConfigOneArea(conf1)
    return render_template('control_area1.html', **templateData)

#--------------------------------------------------------------------------------
# Control Area1 Auto Control
#--------------------------------------------------------------------------------
@app.route("/area1_auto/", methods=['POST'])
def area1_auto():
    
    global conf1
    
    conf1.operation_mode = int(request.form['a_mode'])
    conf1.high_limit = float(request.form['auto_hl'])
    conf1.low_limit = float(request.form['auto_ll'])
    wa4_functions.write_config_files(conf1)      
    os.system('sudo /usr/bin/python3 /home/pi/Code/WA4/WA4_Raspberry/bin/control_loop_area1.py')

    wa4_functions.read_config_files(conf1)    
    templateData=wa4_functions.getConfigOneArea(conf1)
    return render_template('control_area1.html', **templateData)

#--------------------------------------------------------------------------------
# Control Area2
#--------------------------------------------------------------------------------
@app.route("/control_area2/")
def control_area2():
    global conf2

    wa4_functions.read_config_files(conf2)    
    templateData=wa4_functions.getConfigOneArea(conf2)
    return render_template('control_area2.html', **templateData)

#--------------------------------------------------------------------------------
# Control Area2 Operation Mode
#--------------------------------------------------------------------------------
@app.route("/control_area2/<deviceName>")
def control_area2_mode(deviceName):
    global conf2

    if deviceName == 'manual':
        conf2.operation_mode=0
    if deviceName == 'sched':
        conf2.operation_mode=1
    if deviceName == 'auto':
        conf2.operation_mode=2  

    wa4_functions.write_config_files(conf2)      
    templateData=wa4_functions.getConfigOneArea(conf2)
    return render_template('control_area2.html', **templateData)

#--------------------------------------------------------------------------------
# Control Area2 Manual Control
#--------------------------------------------------------------------------------
@app.route("/area2_manual_control/", methods=['POST'])
def area2_manual_control():
    
    global conf2
    conf2.operation_mode = int(request.form['m_mode'])
    conf2.manual_control = int(request.form['control'])
    wa4_functions.write_config_files(conf2)      
    os.system('sudo /usr/bin/python3 /home/pi/Code/WA4/WA4_Raspberry/bin/control_loop_area2.py')

    wa4_functions.read_config_files(conf2)    
    templateData=wa4_functions.getConfigOneArea(conf2)
    return render_template('control_area2.html', **templateData)

#--------------------------------------------------------------------------------
# Control Area2 Sched Control
#--------------------------------------------------------------------------------
@app.route("/area2_sched/", methods=['POST'])
def area2_sched():
    
    global conf2
    
    conf2.operation_mode = int(request.form['s_mode'])
    for i in range (7):
        conf2.day_start_h[i] = int(request.form['start_h'+str(i)])
        conf2.day_start_m[i] = int(request.form['start_m'+str(i)])
        conf2.day_end_h[i] = int(request.form['end_h'+str(i)])
        conf2.day_end_m[i] = int(request.form['end_m'+str(i)])
        conf2.day_of_week[i] = request.form['day'+str(i)]
    wa4_functions.write_config_files(conf2)      
    os.system('sudo /usr/bin/python3 /home/pi/Code/WA4/WA4_Raspberry/bin/control_loop_area2.py')

    wa4_functions.read_config_files(conf2)    
    templateData=wa4_functions.getConfigOneArea(conf2)
    return render_template('control_area2.html', **templateData)

#--------------------------------------------------------------------------------
# Control Area2 Auto Control
#--------------------------------------------------------------------------------
@app.route("/area2_auto/", methods=['POST'])
def area2_auto():
    
    global conf2
    
    conf2.operation_mode = int(request.form['a_mode'])
    conf2.high_limit = float(request.form['auto_hl'])
    conf2.low_limit = float(request.form['auto_ll'])
    wa4_functions.write_config_files(conf2)      
    os.system('sudo /usr/bin/python3 /home/pi/Code/WA4/WA4_Raspberry/bin/control_loop_area2.py')

    wa4_functions.read_config_files(conf2)    
    templateData=wa4_functions.getConfigOneArea(conf2)
    return render_template('control_area2.html', **templateData)

#--------------------------------------------------------------------------------
# Control Area3
#--------------------------------------------------------------------------------
@app.route("/control_area3/")
def control_area3():
    global conf3

    wa4_functions.read_config_files(conf3)    
    templateData=wa4_functions.getConfigOneArea(conf3)
    return render_template('control_area3.html', **templateData)

#--------------------------------------------------------------------------------
# Control Area3 Operation Mode
#--------------------------------------------------------------------------------
@app.route("/control_area3/<deviceName>")
def control_area3_mode(deviceName):
    global conf3

    if deviceName == 'manual':
        conf3.operation_mode=0
    if deviceName == 'sched':
        conf3.operation_mode=1
    if deviceName == 'auto':
        conf3.operation_mode=2  

    wa4_functions.write_config_files(conf3)      
    templateData=wa4_functions.getConfigOneArea(conf3)
    return render_template('control_area3.html', **templateData)

#--------------------------------------------------------------------------------
# Control Area3 Manual Control
#--------------------------------------------------------------------------------
@app.route("/area3_manual_control/", methods=['POST'])
def area3_manual_control():
    
    global conf3
    conf3.operation_mode = int(request.form['m_mode'])
    conf3.manual_control = int(request.form['control'])
    wa4_functions.write_config_files(conf3)      
    os.system('sudo /usr/bin/python3 /home/pi/Code/WA4/WA4_Raspberry/bin/control_loop_area3.py')

    wa4_functions.read_config_files(conf3)    
    templateData=wa4_functions.getConfigOneArea(conf3)
    return render_template('control_area3.html', **templateData)

#--------------------------------------------------------------------------------
# Control Area3 Sched Control
#--------------------------------------------------------------------------------
@app.route("/area3_sched/", methods=['POST'])
def area3_sched():
    
    global conf3
    
    conf3.operation_mode = int(request.form['s_mode'])
    for i in range (7):
        conf3.day_start_h[i] = int(request.form['start_h'+str(i)])
        conf3.day_start_m[i] = int(request.form['start_m'+str(i)])
        conf3.day_end_h[i] = int(request.form['end_h'+str(i)])
        conf3.day_end_m[i] = int(request.form['end_m'+str(i)])
        conf3.day_of_week[i] = request.form['day'+str(i)]
    wa4_functions.write_config_files(conf3)      
    os.system('sudo /usr/bin/python3 /home/pi/Code/WA4/WA4_Raspberry/bin/control_loop_area3.py')

    wa4_functions.read_config_files(conf3)    
    templateData=wa4_functions.getConfigOneArea(conf3)
    return render_template('control_area3.html', **templateData)

#--------------------------------------------------------------------------------
# Control Area3 Auto Control
#--------------------------------------------------------------------------------
@app.route("/area3_auto/", methods=['POST'])
def area3_auto():
    
    global conf3
    
    conf3.operation_mode = int(request.form['a_mode'])
    conf3.high_limit = float(request.form['auto_hl'])
    conf3.low_limit = float(request.form['auto_ll'])
    wa4_functions.write_config_files(conf3)      
    os.system('sudo /usr/bin/python3 /home/pi/Code/WA4/WA4_Raspberry/bin/control_loop_area3.py')

    wa4_functions.read_config_files(conf3)    
    templateData=wa4_functions.getConfigOneArea(conf3)
    return render_template('control_area3.html', **templateData)

#--------------------------------------------------------------------------------
# Control Area4
#--------------------------------------------------------------------------------
@app.route("/control_area4/")
def control_area4():
    global conf4

    wa4_functions.read_config_files(conf4)    
    templateData=wa4_functions.getConfigOneArea(conf4)
    return render_template('control_area4.html', **templateData)

#--------------------------------------------------------------------------------
# Control Area4 Operation Mode
#--------------------------------------------------------------------------------
@app.route("/control_area4/<deviceName>")
def control_area4_mode(deviceName):
    global conf4

    if deviceName == 'manual':
        conf4.operation_mode=0
    if deviceName == 'sched':
        conf4.operation_mode=1
    if deviceName == 'auto':
        conf4.operation_mode=2  

    wa4_functions.write_config_files(conf4)      
    templateData=wa4_functions.getConfigOneArea(conf4)
    return render_template('control_area4.html', **templateData)

#--------------------------------------------------------------------------------
# Control Area4 Manual Control
#--------------------------------------------------------------------------------
@app.route("/area4_manual_control/", methods=['POST'])
def area4_manual_control():
    
    global conf4
    conf4.operation_mode = int(request.form['m_mode'])
    conf4.manual_control = int(request.form['control'])
    wa4_functions.write_config_files(conf4)      
    os.system('sudo /usr/bin/python3 /home/pi/Code/WA4/WA4_Raspberry/bin/control_loop_area4.py')

    wa4_functions.read_config_files(conf4)    
    templateData=wa4_functions.getConfigOneArea(conf4)
    return render_template('control_area4.html', **templateData)

#--------------------------------------------------------------------------------
# Control Area4 Sched Control
#--------------------------------------------------------------------------------
@app.route("/area4_sched/", methods=['POST'])
def area4_sched():
    
    global conf4
    
    conf4.operation_mode = int(request.form['s_mode'])
    for i in range (7):
        conf4.day_start_h[i] = int(request.form['start_h'+str(i)])
        conf4.day_start_m[i] = int(request.form['start_m'+str(i)])
        conf4.day_end_h[i] = int(request.form['end_h'+str(i)])
        conf4.day_end_m[i] = int(request.form['end_m'+str(i)])
        conf4.day_of_week[i] = request.form['day'+str(i)]
    wa4_functions.write_config_files(conf4)      
    os.system('sudo /usr/bin/python3 /home/pi/Code/WA4/WA4_Raspberry/bin/control_loop_area4.py')

    wa4_functions.read_config_files(conf4)    
    templateData=wa4_functions.getConfigOneArea(conf4)
    return render_template('control_area4.html', **templateData)

#--------------------------------------------------------------------------------
# Control Area4 Auto Control
#--------------------------------------------------------------------------------
@app.route("/area4_auto/", methods=['POST'])
def area4_auto():
    
    global conf4
    
    conf4.operation_mode = int(request.form['a_mode'])
    conf4.high_limit = float(request.form['auto_hl'])
    conf4.low_limit = float(request.form['auto_ll'])
    wa4_functions.write_config_files(conf4)      
    os.system('sudo /usr/bin/python3 /home/pi/Code/WA4/WA4_Raspberry/bin/control_loop_area4.py')

    wa4_functions.read_config_files(conf4)    
    templateData=wa4_functions.getConfigOneArea(conf4)
    return render_template('control_area4.html', **templateData)

#--------------------------------------------------------------------------------
# Monitoreo Climático
#--------------------------------------------------------------------------------
@app.route("/weather_index/")
def weather_index():
    global host_ip, nodered_port
    print("Redirecting to http://"+host_ip+":"+nodered_port+"/ui/#!/5/")
    return redirect("http://"+host_ip+":"+nodered_port+"/ui/#!/5/")

#--------------------------------------------------------------------------------
# Acceso a Datos
#--------------------------------------------------------------------------------
@app.route("/data_index/")
def data_index():
    return render_template('data_index.html')


#--------------------------------------------------------------------------------
# Datos Area 1
#--------------------------------------------------------------------------------
@app.route("/data1/")
def data1():
    return send_file("./wa4_log/log_area1.csv", as_attachment=True)

#--------------------------------------------------------------------------------
# Datos Area 2
#--------------------------------------------------------------------------------
@app.route("/data2/")
def data2():
    return send_file("./wa4_log/log_area2.csv", as_attachment=True)

#--------------------------------------------------------------------------------
# Datos Area 3
#--------------------------------------------------------------------------------
@app.route("/data3/")
def data3():
    return send_file("./wa4_log/log_area3.csv", as_attachment=True)

#--------------------------------------------------------------------------------
# Datos Area 4
#--------------------------------------------------------------------------------
@app.route("/data4/")
def data4():
    return send_file("./wa4_log/log_area4.csv", as_attachment=True)

#--------------------------------------------------------------------------------
# Acceso a Graficas
#--------------------------------------------------------------------------------
@app.route("/graphics_index/")
def graphics_index():
    global host_ip, nodered_port
    print("Redirecting to http://"+host_ip+":"+nodered_port+"/ui/#!/6/")
    return redirect("http://"+host_ip+":"+nodered_port+"/ui/#!/6/")

#--------------------------------------------------------------------------------
# Acceso a Graficas Area 1
#--------------------------------------------------------------------------------
@app.route("/graphics_area1/")
def graphics_area1():
    global host_ip, nodered_port
    print("Redirecting to http://"+host_ip+":"+nodered_port+"/ui/#!/7/")
    return redirect("http://"+host_ip+":"+nodered_port+"/ui/#!/7/")

#--------------------------------------------------------------------------------
# Acceso a Graficas Area 2
#--------------------------------------------------------------------------------
@app.route("/graphics_area2/")
def graphics_area2():
    global host_ip, nodered_port
    print("Redirecting to http://"+host_ip+":"+nodered_port+"/ui/#!/8/")
    return redirect("http://"+host_ip+":"+nodered_port+"/ui/#!/8/")

#--------------------------------------------------------------------------------
# Acceso a Graficas Area 3
#--------------------------------------------------------------------------------
@app.route("/graphics_area3/")
def graphics_area3():
    global host_ip, nodered_port
    print("Redirecting to http://"+host_ip+":"+nodered_port+"/ui/#!/9/")
    return redirect("http://"+host_ip+":"+nodered_port+"/ui/#!/9/")

#--------------------------------------------------------------------------------
# Acceso a Graficas Area 4
#--------------------------------------------------------------------------------
@app.route("/graphics_area4/")
def graphics_area4():
    global host_ip, nodered_port
    print("Redirecting to http://"+host_ip+":"+nodered_port+"/ui/#!/10/")
    return redirect("http://"+host_ip+":"+nodered_port+"/ui/#!/10/")

#--------------------------------------------------------------------------------
# Acceso a Graficas Climático
#--------------------------------------------------------------------------------
@app.route("/graphics_weather/")
def graphics_weather():
    global host_ip, nodered_port
    print("Redirecting to http://"+host_ip+":"+nodered_port+"/ui/#!/11/")
    return redirect("http://"+host_ip+":"+nodered_port+"/ui/#!/11/")

#--------------------------------------------------------------------------------
# Thing Speak
#--------------------------------------------------------------------------------
##@app.route("/general_riego/<deviceName>")
##def general_riego(deviceName):
##
##    if deviceName == 'days=3':
##        plot_days=3
##    if deviceName == 'days=7':
##        plot_days=7
##    if deviceName == 'days=15':
##        plot_days=15
##    if deviceName == 'days=30':
##        plot_days=30
##    if deviceName == 'days=90':
##        plot_days=90
##
##    templateData = {
##        'plot_days' : plot_days,
##    }
##    return render_template('thingspeak_general_riego.html', **templateData)

#--------------------------------------------------------------------------------
# Thing Speak
#--------------------------------------------------------------------------------
##@app.route("/general_sensores/<deviceName>")
##def general_sensores(deviceName):
##
##    if deviceName == 'days=3':
##        plot_days=3
##    if deviceName == 'days=7':
##        plot_days=7
##    if deviceName == 'days=15':
##        plot_days=15
##    if deviceName == 'days=30':
##        plot_days=30
##    if deviceName == 'days=90':
##        plot_days=90
##
##    templateData = {
##        'plot_days' : plot_days,
##    }
##    return render_template('thingspeak_general_sensores.html', **templateData)

#--------------------------------------------------------------------------------
# Thing Speak
#--------------------------------------------------------------------------------
##@app.route("/area1_riego/<deviceName>")
##def area1_riego(deviceName):
##
##    if deviceName == 'days=3':
##        plot_days=3
##    if deviceName == 'days=7':
##        plot_days=7
##    if deviceName == 'days=15':
##        plot_days=15
##    if deviceName == 'days=30':
##        plot_days=30
##    if deviceName == 'days=90':
##        plot_days=90
##
##    templateData = {
##        'plot_days' : plot_days,
##    }
##    return render_template('thingspeak_area1_riego.html', **templateData)

#--------------------------------------------------------------------------------
# Thing Speak
#--------------------------------------------------------------------------------
##@app.route("/area1_sensores/<deviceName>")
##def area1_sensores(deviceName):
##
##    if deviceName == 'days=3':
##        plot_days=3
##    if deviceName == 'days=7':
##        plot_days=7
##    if deviceName == 'days=15':
##        plot_days=15
##    if deviceName == 'days=30':
##        plot_days=30
##    if deviceName == 'days=90':
##        plot_days=90
##
##    templateData = {
##        'plot_days' : plot_days,
##    }
##    return render_template('thingspeak_area1_sensores.html', **templateData)

#--------------------------------------------------------------------------------
# Thing Speak
#--------------------------------------------------------------------------------
##@app.route("/area2_riego/<deviceName>")
##def area2_riego(deviceName):
##
##    if deviceName == 'days=3':
##        plot_days=3
##    if deviceName == 'days=7':
##        plot_days=7
##    if deviceName == 'days=15':
##        plot_days=15
##    if deviceName == 'days=30':
##        plot_days=30
##    if deviceName == 'days=90':
##        plot_days=90
##
##    templateData = {
##        'plot_days' : plot_days,
##    }
##    return render_template('thingspeak_area2_riego.html', **templateData)

#--------------------------------------------------------------------------------
# Thing Speak
#--------------------------------------------------------------------------------
##@app.route("/area2_sensores/<deviceName>")
##def area2_sensores(deviceName):
##
##    if deviceName == 'days=3':
##        plot_days=3
##    if deviceName == 'days=7':
##        plot_days=7
##    if deviceName == 'days=15':
##        plot_days=15
##    if deviceName == 'days=30':
##        plot_days=30
##    if deviceName == 'days=90':
##        plot_days=90
##
##    templateData = {
##        'plot_days' : plot_days,
##    }
##    return render_template('thingspeak_area2_sensores.html', **templateData)

#--------------------------------------------------------------------------------
# Thing Speak
#--------------------------------------------------------------------------------
##@app.route("/area3_riego/<deviceName>")
##def area3_riego(deviceName):
##
##    if deviceName == 'days=3':
##        plot_days=3
##    if deviceName == 'days=7':
##        plot_days=7
##    if deviceName == 'days=15':
##        plot_days=15
##    if deviceName == 'days=30':
##        plot_days=30
##    if deviceName == 'days=90':
##        plot_days=90
##
##    templateData = {
##        'plot_days' : plot_days,
##    }
##    return render_template('thingspeak_area3_riego.html', **templateData)

#--------------------------------------------------------------------------------
# Thing Speak
#--------------------------------------------------------------------------------
##@app.route("/area3_sensores/<deviceName>")
##def area3sensores(deviceName):
##
##    if deviceName == 'days=3':
##        plot_days=3
##    if deviceName == 'days=7':
##        plot_days=7
##    if deviceName == 'days=15':
##        plot_days=15
##    if deviceName == 'days=30':
##        plot_days=30
##    if deviceName == 'days=90':
##        plot_days=90
##
##    templateData = {
##        'plot_days' : plot_days,
##    }
##    return render_template('thingspeak_area3_sensores.html', **templateData)

#--------------------------------------------------------------------------------
# Thing Speak
#--------------------------------------------------------------------------------
##@app.route("/area4_riego/<deviceName>")
##def area4_riego(deviceName):
##
##    if deviceName == 'days=3':
##        plot_days=3
##    if deviceName == 'days=7':
##        plot_days=7
##    if deviceName == 'days=15':
##        plot_days=15
##    if deviceName == 'days=30':
##        plot_days=30
##    if deviceName == 'days=90':
##        plot_days=90
##
##    templateData = {
##        'plot_days' : plot_days,
##    }
##    return render_template('thingspeak_area4_riego.html', **templateData)

#--------------------------------------------------------------------------------
# Thing Speak
#--------------------------------------------------------------------------------
##@app.route("/area4_sensores/<deviceName>")
##def area4_sensores(deviceName):
##
##    if deviceName == 'days=3':
##        plot_days=3
##    if deviceName == 'days=7':
##        plot_days=7
##    if deviceName == 'days=15':
##        plot_days=15
##    if deviceName == 'days=30':
##        plot_days=30
##    if deviceName == 'days=90':
##        plot_days=90
##
##    templateData = {
##        'plot_days' : plot_days,
##    }
##    return render_template('thingspeak_area4_sensores.html', **templateData)

#--------------------------------------------------------------------------------
# Obtain local host ip address
#--------------------------------------------------------------------------------
s1=subprocess.check_output(["hostname","-I"])
s2=s1.decode("utf-8")
host_ip=s2.strip(" \n")
print("Localhost IP Addresss: " + host_ip)

#--------------------------------------------------------------------------------
# Main Routine
#--------------------------------------------------------------------------------
if __name__ == "__main__":
   #Run web server
   app.run(host='0.0.0.0', port=80, debug=True, threaded=True) 
