#!/usr/bin/env python

import paho.mqtt.publish as publish


#---------------------------------------------------------------
# Update node red plots irrigation 
#---------------------------------------------------------------
def update_irrigation(data):
        MQTT_SERVER = "localhost"
        publish.single("wa/area"+str(data.area_id)+"/smc", str(data.smC), hostname=MQTT_SERVER)
        publish.single("wa/area"+str(data.area_id)+"/sm1", str(data.sm1), hostname=MQTT_SERVER)
        publish.single("wa/area"+str(data.area_id)+"/sm2", str(data.sm2), hostname=MQTT_SERVER)
        publish.single("wa/area"+str(data.area_id)+"/sm3", str(data.sm3), hostname=MQTT_SERVER)
        publish.single("wa/area"+str(data.area_id)+"/valve", str(data.valve_status), hostname=MQTT_SERVER)
        publish.single("wa/area"+str(data.area_id)+"/flow", str(data.valve_flow), hostname=MQTT_SERVER)
        return

#---------------------------------------------------------------
# Update node red plots eto
#---------------------------------------------------------------
def update_weather(data):
        MQTT_SERVER = "localhost"
        publish.single("wa/weather/eto", str(data.w_eto), hostname=MQTT_SERVER)
        publish.single("wa/weather/solar", str(data.w_radiation), hostname=MQTT_SERVER)
        publish.single("wa/weather/temp", str(data.w_temperature), hostname=MQTT_SERVER)
        publish.single("wa/weather/hum", str(data.w_humidity), hostname=MQTT_SERVER)
        publish.single("wa/weather/wind",str(data.w_wind), hostname=MQTT_SERVER)
        return
