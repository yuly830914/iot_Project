import time  
import sys  
import httplib, urllib  
import json

sys.path.insert(0, '/usr/lib/python2.7/bridge/')  
from bridgeclient import BridgeClient as bridgeclient

value = bridgeclient()

deviceId = "DFGcNP8I"  
deviceKey = "PJK5CClSL6CZ20Pp"

def post_to_mcs(payload):  
    headers = {"Content-type": "application/json", "deviceKey": deviceKey}
    not_connected = 1
    while (not_connected):
        try:
            conn = httplib.HTTPConnection("api.mediatek.com:80")
            conn.connect()
            not_connected = 0
        except (httplib.HTTPException, socket.error) as ex:
            print "Error: %s" % ex

    conn.request("POST", "/mcs/v2/devices/" + deviceId + "/datapoints", json.dumps(payload), headers)
    response = conn.getresponse()
    print( response.status, time.strftime("%c"))
    data = response.read()
    conn.close()

while True:  
    # vibrate = value.get("vibrate")
    yaw = value.get("yaw")
    pitch = value.get("pitch")
    roll = value.get("roll")
    accelX = value.get("accelX")
    accelY = value.get("accelY")
    accelZ = value.get("accelZ")
    gyroX = value.get("gyroX")
    gyroY = value.get("gyroY")
    gyroZ = value.get("gyroZ")

    payload = {"datapoints":[
    				# {"dataChnId":"vibrate","values":{"value":vibrate}},
    				{"dataChnId":"yaw","values":{"value":yaw}},
    				{"dataChnId":"pitch","values":{"value":pitch}},
    				{"dataChnId":"roll","values":{"value":roll}},
    				{"dataChnId":"accelX","values":{"value":accelX}},
    				{"dataChnId":"accelY","values":{"value":accelY}},
    				{"dataChnId":"accelZ","values":{"value":accelZ}},
    				{"dataChnId":"gyroX","values":{"value":gyroX}},
    				{"dataChnId":"gyroY","values":{"value":gyroY}},
    				{"dataChnId":"gyroZ","values":{"value":gyroZ}}
    		  ]}
    post_to_mcs(payload)

