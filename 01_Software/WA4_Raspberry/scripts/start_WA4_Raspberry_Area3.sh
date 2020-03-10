cd /home/pi/Code/WA4/WA4_Raspberry/bin
echo "----------------------------------------" >> /home/pi/Code/WA4/WA4_Raspberry/log/log_crontab.txt
echo "Starting WA4 Area 3 control loop process" >> /home/pi/Code/WA4/WA4_Raspberry/log/log_crontab.txt
date >> /home/pi/Code/WA4/WA4_Raspberry/log/log_crontab.txt
sudo /usr/bin/python3 /home/pi/Code/WA4/WA4_Raspberry/bin/control_loop_area3.py >> /home/pi/Code/WA4/WA4_Raspberry/log/log_crontab.txt
echo "Ending WA4 Area 3 control loop process" >> /home/pi/Code/WA4/WA4_Raspberry/log/log_crontab.txt
date >> /home/pi/Code/WA4/WA4_Raspberry/log/log_crontab.txt

