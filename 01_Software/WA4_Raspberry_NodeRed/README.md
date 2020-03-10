# WA4_Raspberry
# Updated Sep-2019

Directories:

bin 	- python executable programs
config 	- csv configuration files
data 	- latest data from sensors and actuators
log 	- output log files
scripts - scripts for crontab and boot
tools	- program tools to check communication with nodes
www_flask - web interface



Files

bin\control_loop_area1.py	- main file for irrigation area 1
bin\control_loop_area2.py	- main file for irrigation area 2
bin\control_loop_area3.py	- main file for irrigation area 3
bin\control_loop_area4.py	- main file for irrigation area 4
bin\wa_config_data.py		- functions to access configuration files
bin\wa_control.py		- functions for control and data fusion algorithms
bin\wa_xbee_comm.py		- function for Zigbee communication
bin\wa_iot_comm.py		- function for IoT interface (ThingSpeak)

config\config_area1.csv		- configuration file for irrigation area 1
config\config_area2.csv		- configuration file for irrigation area 2
config\config_area3.csv		- configuration file for irrigation area 3
config\config_area4.csv		- configuration file for irrigation area 4
config\manual_control_area1.csv	- configuration file for manual irrigation area 1
config\manual_control_area2.csv	- configuration file for manual irrigation area 2
config\manual_control_area3.csv	- configuration file for manual irrigation area 3
config\manual_control_area4.csv	- configuration file for manual irrigation area 4
config\sched_area1.csv		- configuration file for scheduling irrigation area 1
config\sched_area2.csv		- configuration file for scheduling irrigation area 2
config\sched_area3.csv		- configuration file for scheduling irrigation area 3
config\sched_area4.csv		- configuration file for scheduling irrigation area 4
config\operation_mode_area1.csv	- confirguration file for operation mode area 1
config\operation_mode_area2.csv	- confirguration file for operation mode area 2
config\operation_mode_area3.csv	- confirguration file for operation mode area 3
config\operation_mode_area4.csv	- confirguration file for operation mode area 4

data\data_area1.csv		- Current data for irrigation area 1
data\data_area2.csv		- Current data for irrigation area 2
data\data_area3.csv		- Current data for irrigation area 3
data\data_area4.csv		- Current data for irrigation area 4

log\crontab.txt			- log data for coontab services
log\video.txt			- log data for video server
log\webiopi.txt			- log data for webiopi server
log\webfalsk.txt		- log data for flask server

scripts\cron.txt		- cron process definition, start each irrigation area monitoring every 5 minutes
scripts\start_crontab.sh	- script to start crontab process (once executed, onlye removed by command. crontab -r
scripts\start_mjpg_streamer.sh	- start video server during startup (boot), to configure file must be copied to directory /etc/init.d/
scripts\start_WebFlaskServer.sh	- start web falsk server during startup (boot), to configure file must be copied to directory /etc/init.d/
scripts\start_WA4_Raspberry_All_Areas.sh	-execute monitor and control for all areas
scripts\start_WA4_Raspberry_Area1.sh		-execute monitor and control for area 1
scripts\start_WA4_Raspberry_Area2.sh		-execute monitor and control for area 2
scripts\start_WA4_Raspberry_Area3.sh		-execute monitor and control for area 3
scripts\start_WA4_Raspberry_Area4.sh		-execute monitor and control for area 4

tools\actuator_message.py	- Program to check communication with actuator node
tools\sensor_message.py		- Program to check communication with sensor node
tools\weather_message.py	- Program to check communication with weather node

www_flask\runWebServer.py	- Main flask program
www_flask\wa4_functions.py	- Complementary flask functions
www_flask\static\		- Javascript and css files
www_flask\templates\		- HTML files for flask
www_flask\wa4_log\log_area1.csv	- Historic data for irrigation area 1
www_flask\wa4_log\log_area2.csv	- Historic data for irrigation area 2
www_flask\wa4_log\log_area3.csv	- Historic data for irrigation area 3
www_flask\wa4_log\log_area4.csv	- Historic data for irrigation area 4




