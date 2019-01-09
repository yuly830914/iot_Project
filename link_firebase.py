# import Package

import sys
import time
import datetime
import requests
import json

# Setting Firebse URL, Date, Time

firebase_url = 'https://smart-sensor-tag.firebaseio.com/'

# Get Data from Bridge

sys.path.insert(0, '/usr/lib/python2.7/bridge/')
from bridgeclient import BridgeClient as bridgeclient
value = bridgeclient()

# Insert Data
def upload_to_firebase(data, tag):
	result = requests.post(firebase_url + '/' + tag + '.json', data=json.dumps(data))
	print 'Status Code = ' + str(result.status_code) + ', Response = ' + result.text
	print data


while True:
	# read data
	tag = value.get("tag")
	vibrate = value.get("vibrate")
	yaw = value.get("yaw")
	pitch = value.get("pitch")
	roll = value.get("roll")
	accelX = value.get("accelX")
	accelY = value.get("accelY")
	accelZ = value.get("accelZ")
	gyroX = value.get("gyroX")
	gyroY = value.get("gyroY")
	gyroZ = value.get("gyroZ")

	t = time.time();
	date = datetime.datetime.fromtimestamp(t).strftime('%Y%m%d%H%M%S')

	data = {'timestamp':date,
			'vibrate':vibrate, 
			'yaw':yaw, 		 
			'pitch':pitch,	  
			'roll':roll, 
			'accelX':accelX, 
			'accelY':accelY, 
			'accelZ':accelZ,
			'gyroX':gyroX,	 
			'gyroY':gyroY,   
			'gyroZ':gyroZ}

	upload_to_firebase(data, tag)


