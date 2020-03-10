#!/bin/sh
### BEGIN INIT INFO
# Provides:          webiopi
# Required-Start:    $local_fs $remote_fs $network $syslog $named
# Required-Stop:     $local_fs $remote_fs $network $syslog $named
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# X-Interactive:     true
# Short-Description: webiopi web server
# Description:       Start the web server and associated helpers
#  This script will start webiopi, and possibly all associated instances.
### END INIT INFO

date >> /home/pi/Code/WA4/WA4_Raspberry/log/log_webiopi.txt
echo "Starting webiopi services" >> /home/pi/Code/WA4/WA4_Raspberry/log/log_webiopi.txt
sudo webiopi -d -c /etc/webiopi/config >> /home/pi/Code/WA4/WA4_Raspberry/log/log_webiopi.txt
