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
		temperatureEntry = {'latitude':entry["latitude"],'longitude':entry["longitude"],'temperature':entry["temperature"],'timestamp':entry["time"]}
		temperatureData.append(temperatureEntry)
	
	keyWindSpeed = "windSpeed"
	keyWindBearing = "windBearing"
	if keyWindSpeed and keyWindBearing in entry:
		windEntry = {'latitude':entry["latitude"],'longitude':entry["longitude"],'speed':entry["windSpeed"],'direction':entry["windBearing"],'timestamp':entry["time"]}
		windData.append(windEntry)
	
	keyCloudCover = "cloudCover"
	if keyCloudCover in entry:
		cloudCoverEntry = {'latitude':entry["latitude"],'longitude':entry["longitude"],'cloudCover':entry["cloudCover"],'timestamp':entry["time"]}
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
		precipitationEntry = {'latitude':entry["latitude"],'longitude':entry["longitude"],'precipitationRate':entry["precipIntensity"],'precipitationType':entry["precipType"],'timestamp':entry["time"]}
		precipitationData.append(precipitationEntry)
	elif type == False and intens == True:
		precipitationEntry = {'latitude':entry["latitude"],'longitude':entry["longitude"],'precipitationRate':entry["precipIntensity"],'timestamp':entry["time"]}
		precipitationData.append(precipitationEntry)

# adds a header to each DataArray	
temperatureDict = {'meta': {'dataType': 'temperature', 'temporal': "true"}, 'data': temperatureData}
windDict = {'meta': {'dataType': 'wind', 'temporal': "true"}, 'data': windData}
cloudCoverDict = {'meta': {'dataType': 'cloudCover', 'temporal': "true"}, 'data': cloudCoverData}
precipitationDict = {'meta': {'dataType': 'precipitation', 'temporal': "true"}, 'data': precipitationData}

# writes the different kJson-files for the different weatherSources	
d.write_json_file(temperatureDict, 'temperature-data.kJson', True)
d.write_json_file(windDict, 'wind-data.kJson', True)
d.write_json_file(cloudCoverDict, 'cloudCover-data.kJson', True)
d.write_json_file(precipitationDict, 'precipitation-data.kJson', True)