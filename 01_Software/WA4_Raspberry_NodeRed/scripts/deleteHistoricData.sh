cd /home/pi/Code/WA4/WA4_Raspberry/www_flask/wa4_log
mv log_area1.csv ./Historic/log_area1_$(date +%Y-%m-%d_%H_%M_%S).csv
mv log_area2.csv ./Historic/log_area2_$(date +%Y-%m-%d_%H_%M_%S).csv
mv log_area3.csv ./Historic/log_area3_$(date +%Y-%m-%d_%H_%M_%S).csv
mv log_area4.csv ./Historic/log_area4_$(date +%Y-%m-%d_%H_%M_%S).csv
echo "DATE,SMC,SM1,SM2,SM3,V,FLOW,RAD,TEMP,HUM,WIND,ETO,ALPHA,NDVI,RED,NIR,E" > ./log_area1.csv
echo "DATE,SMC,SM1,SM2,SM3,V,FLOW,RAD,TEMP,HUM,WIND,ETO,ALPHA,NDVI,RED,NIR,E" > ./log_area2.csv
echo "DATE,SMC,SM1,SM2,SM3,V,FLOW,RAD,TEMP,HUM,WIND,ETO,ALPHA,NDVI,RED,NIR,E" > ./log_area3.csv
echo "DATE,SMC,SM1,SM2,SM3,V,FLOW,RAD,TEMP,HUM,WIND,ETO,ALPHA,NDVI,RED,NIR,E" > ./log_area4.csv
chmod 775 ./log_area1.csv
chmod 775 ./log_area2.csv
chmod 775 ./log_area3.csv
chmod 775 ./log_area4.csv
