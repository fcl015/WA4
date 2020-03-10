#!/bin/sh
### BEGIN INIT INFO
# Provides:          webflask
# Required-Start:    $local_fs $remote_fs $network $syslog $named
# Required-Stop:     $local_fs $remote_fs $network $syslog $named
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# X-Interactive:     true
# Short-Description: webflask web server
# Description:       Start the web server and associated helpers
#  This script will start webiopi, and possibly all associated instances.
### END INIT INFO

date >> /home/pi/Code/WA4/WA4_Raspberry/log/log_webflask.txt
echo "Starting Web Flask Server services" >> /home/pi/Code/WA4/WA4_Raspberry/log/log_webflask.txt
cd /home/pi/Code/WA4/WA4_Raspberry/www_flask 
sudo lxterminal -t WebServer -e python3 runWebServer.py
