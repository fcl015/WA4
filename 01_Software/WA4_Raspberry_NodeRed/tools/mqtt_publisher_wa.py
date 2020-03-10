import paho.mqtt.publish as publish
 
MQTT_SERVER = "localhost"
 
publish.single("wa/area1/smc", "48.3", hostname=MQTT_SERVER)
publish.single("wa/area2/smc", "46.4", hostname=MQTT_SERVER)
publish.single("wa/area3/smc", "47.1", hostname=MQTT_SERVER)
publish.single("wa/area4/smc", "49.4", hostname=MQTT_SERVER)

publish.single("wa/area1/valve", "1", hostname=MQTT_SERVER)
publish.single("wa/area2/valve", "1", hostname=MQTT_SERVER)
publish.single("wa/area3/valve", "1", hostname=MQTT_SERVER)
publish.single("wa/area4/valve", "1", hostname=MQTT_SERVER)

publish.single("wa/area1/flow", "22.0", hostname=MQTT_SERVER)
publish.single("wa/area2/flow", "33.0", hostname=MQTT_SERVER)
publish.single("wa/area3/flow", "44.0", hostname=MQTT_SERVER)
publish.single("wa/area4/flow", "40.0", hostname=MQTT_SERVER)


publish.single("wa/weather/eto", "5.2", hostname=MQTT_SERVER)
publish.single("wa/weather/solar", "533.0", hostname=MQTT_SERVER)
publish.single("wa/weather/temp", "12.3", hostname=MQTT_SERVER)
publish.single("wa/weather/hum", "28.6", hostname=MQTT_SERVER)
publish.single("wa/weather/wind", "1.3", hostname=MQTT_SERVER)

publish.single("wa/area1/sm1", "33.1", hostname=MQTT_SERVER)
publish.single("wa/area2/sm1", "35.2", hostname=MQTT_SERVER)
publish.single("wa/area3/sm1", "38.3", hostname=MQTT_SERVER)
publish.single("wa/area4/sm1", "39.4", hostname=MQTT_SERVER)

publish.single("wa/area1/sm2", "43.1", hostname=MQTT_SERVER)
publish.single("wa/area2/sm2", "45.2", hostname=MQTT_SERVER)
publish.single("wa/area3/sm2", "48.3", hostname=MQTT_SERVER)
publish.single("wa/area4/sm2", "49.4", hostname=MQTT_SERVER)

publish.single("wa/area1/sm3", "23.1", hostname=MQTT_SERVER)
publish.single("wa/area2/sm3", "25.2", hostname=MQTT_SERVER)
publish.single("wa/area3/sm3", "28.3", hostname=MQTT_SERVER)
publish.single("wa/area4/sm3", "29.4", hostname=MQTT_SERVER)
