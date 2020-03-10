#!/usr/bin/env python

import time
import requests


#---------------------------------------------------------------
# Update thingspeak
#---------------------------------------------------------------
def update_thingspeak(data,api_key):

        r=requests.post("http://api.thingspeak.com/update",data={'key':api_key,
                'field1':str(data.smC),'field2':str(data.valve_status),'field3':str(data.w_radiation),'field4':str(data.w_temperature),
                'field5':str(data.w_humidity),'field6':str(data.w_wind),'field7':str(data.w_eto),'field8':str(data.ndvi_value)})
        #if data.verbose:
        print(r)

        return r


#---------------------------------------------------------------
# Update thingspeak area1
#---------------------------------------------------------------
def update_thingspeak_area1(data):

        r=requests.post("http://api.thingspeak.com/update",data={'key':'F1FVCMK7D72DMWIM',
                'field1':str(data.smC),'field2':str(data.sm1),
                'field3':str(data.sm2),'field4':str(data.sm3),
                'field5':str(data.valve_status),'field6':str(data.valve_flow),
                'field7':str(data.w_eto),'field8':str(data.w_temperature)})
        #if data.verbose:
        print(r)

        return r

#---------------------------------------------------------------
# Update thingspeak area2
#---------------------------------------------------------------
def update_thingspeak_area2(data):

        r=requests.post("http://api.thingspeak.com/update",data={'key':'6J7J3MKA6G6G3XTK',
                'field1':str(data.smC),'field2':str(data.sm1),
                'field3':str(data.sm2),'field4':str(data.sm3),
                'field5':str(data.valve_status),'field6':str(data.valve_flow),
                'field7':str(data.w_eto),'field8':str(data.w_humidity)})
        #if data.verbose:
        print(r)

        return r

#---------------------------------------------------------------
# Update thingspeak area3
#---------------------------------------------------------------
def update_thingspeak_area3(data):

        r=requests.post("http://api.thingspeak.com/update",data={'key':'B864OCW44T9A6IHX',
                'field1':str(data.smC),'field2':str(data.sm1),
                'field3':str(data.sm2),'field4':str(data.sm3),
                'field5':str(data.valve_status),'field6':str(data.valve_flow),
                'field7':str(data.w_eto),'field8':str(data.w_radiation)})
        #if data.verbose:
        print(r)

        return r
#---------------------------------------------------------------
# Update thingspeak area4
#---------------------------------------------------------------
def update_thingspeak_area4(data):

        r=requests.post("http://api.thingspeak.com/update",data={'key':'6UYUE7C73XOR5WD6',
                'field1':str(data.smC),'field2':str(data.sm1),
                'field3':str(data.sm2),'field4':str(data.sm3),
                'field5':str(data.valve_status),'field6':str(data.valve_flow),
                'field7':str(data.w_eto),'field8':str(data.w_wind)})
        #if data.verbose:
        print(r)

        return r
