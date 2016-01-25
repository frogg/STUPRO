import json
from DataUtility import DataUtility

d = DataUtility()
weather_source = d.read_json_file('weather-data-collection.json')
weatherData = weather_source["data"]

temperatureData = []
windData = []
cloudCoverData = []
precipitationData = []

# goes through all Datapoints in the mixed weather-data-collection,
# splits the Data into the different weatherSources and finally writes
# them into different Data-Arrays
for index, entry in enumerate(weatherData):
	# not every entry has every key included. so every key is checked first
	keyTemperature = "temperature"
	if keyTemperature in entry:
		temperatureEntry = {'latitude':entry["latitude"],'longitude':entry["longitude"],'temperature':entry["temperature"],'timestamp':entry["time"],"children": []}
		temperatureData.append(temperatureEntry)
	
	keyWindSpeed = "windSpeed"
	keyWindBearing = "windBearing"
	if keyWindSpeed and keyWindBearing in entry:
		windEntry = {'latitude':entry["latitude"],'longitude':entry["longitude"],'speed':entry["windSpeed"],'direction':entry["windBearing"],'timestamp':entry["time"],"children": []}
		windData.append(windEntry)
	
	keyCloudCover = "cloudCover"
	if keyCloudCover in entry:
		cloudCoverEntry = {'latitude':entry["latitude"],'longitude':entry["longitude"],'cloudCover':entry["cloudCover"],'timestamp':entry["time"],"children": []}
	cloudCoverData.append(cloudCoverEntry)
	
	keyPrecipitationType = "precipType"
	keyPrecipitationIntensity = "precipIntensity"
	type = False
	intens = False
	if keyPrecipitationType in entry:
		type = True
	if keyPrecipitationIntensity in entry:
		intens = True
	if type == True and intens == True:
		precipitationEntry = {'latitude':entry["latitude"],'longitude':entry["longitude"],'precipitationRate':entry["precipIntensity"],'precipitationType':entry["precipType"],'timestamp':entry["time"],"children": []}
		precipitationData.append(precipitationEntry)
	elif type == False and intens == True:
		#precipitationEntry = {'latitude':entry["latitude"],'longitude':entry["longitude"],'precipitationRate':entry["precipIntensity"],'timestamp':entry["time"]}
		precipitationEntry = {'latitude':entry["latitude"],'longitude':entry["longitude"],'precipitationRate':entry["precipIntensity"],'precipitationType':"NoData",'timestamp':entry["time"],"children": []}
		precipitationData.append(precipitationEntry)

# adds a header to each DataArray	
timeResolution = 1 * 60 * 60

temperatureDict = {'meta': {'dataType': 'temperature', 'temporal': True, 'timeResolution': timeResolution}, 'root':{"children":temperatureData}}
windDict = {'meta': {'dataType': 'wind', 'temporal': True, 'timeResolution': timeResolution}, 'root':{"children":windData}}
cloudCoverDict = {'meta': {'dataType': 'cloudCover', 'temporal': True, 'timeResolution': timeResolution}, 'root':{"children":cloudCoverData}}
precipitationDict = {'meta': {'dataType': 'precipitation', 'temporal': True, 'timeResolution': timeResolution}, 'root':{"children":precipitationData}}

# writes the different kJson-files for the different weatherSources	
d.write_json_file(temperatureDict, 'test-data/temperature-data.kJson', True)
d.write_json_file(windDict, 'test-data/wind-data.kJson', True)
d.write_json_file(cloudCoverDict, 'test-data/cloudCover-data.kJson', True)
d.write_json_file(precipitationDict, 'test-data/precipitation-data.kJson', True)