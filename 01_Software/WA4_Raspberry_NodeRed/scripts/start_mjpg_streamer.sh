#!/bin/sh
### BEGIN INIT INFO
# Provides:          mjpg streamer
# Required-Start:    $local_fs $remote_fs $network $syslog $named
# Required-Stop:     $local_fs $remote_fs $network $syslog $named
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# X-Interactive:     true
# Short-Description: mjpg video server
# Description:       Start the video server and associated helpers
#  This script will start mjpg streamer, and possibly all associated instances.
### END INIT INFO
cd /home/pi/mjpg-streamer/mjpg-streamer-experimental
date >> /home/pi/Code/WA4/WA4_Raspberry/log/log_video.txt
echo "Starting mjpg-streamer video services" >> /home/pi/Code/WA4/WA4_Raspberry/log/log_video.txt
sudo /home/pi/mjpg-streamer/mjpg-streamer-experimental/mjpg_streamer -o "output_http.so -w ./www" -i "input_raspicam.so" >> /home/pi/Code/WA4/WA4_Raspberry/log/log_video.txt
