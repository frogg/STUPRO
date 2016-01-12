import os.path
import json
import urllib2
import requests
import datetime
import time
from DataUtility import DataUtility

config_file_name = 'forecastio.config'

def get_api_key():
    if os.path.isfile(config_file_name) and len(open('forecastio.config', 'rb').read().splitlines()) > 0:
        return open('forecastio.config', 'rb').read().splitlines()[0]
    else:
        raise Exception('The configuration file \'{0}\' containing your API key is invalid or does not exist.'.format(config_file_name))




d = DataUtility()
weather_config = d.read_json_file('weather-data-collection.json')


start_time = int(time.mktime(datetime.datetime.strptime(weather_config["meta"]["dateRange"]["start"], "%Y-%m-%dT00:00:00.000Z").timetuple()))
end_time = int(time.mktime(datetime.datetime.strptime(weather_config["meta"]["dateRange"]["end"], "%Y-%m-%dT00:00:00.000Z").timetuple()))


start_lat = weather_config["meta"]["geographicRange"]["upperLeft"]["latitude"]
start_lon = weather_config["meta"]["geographicRange"]["upperLeft"]["longitude"]

end_lat = weather_config["meta"]["geographicRange"]["bottomRight"]["latitude"]
end_lon = weather_config["meta"]["geographicRange"]["bottomRight"]["longitude"]

globallyDownloadedSets = weather_config["meta"]["downloadedSets"]

currentlyDownloadedDataSets = 0




current_longitude = start_lon
while current_longitude >= end_lon:
    print "Long", current_longitude

    current_latitude = start_lat
    while current_latitude <= end_lat:
        print "Lat", current_latitude
        current_time = start_time
        while current_time <= end_time:
            if currentlyDownloadedDataSets >= globallyDownloadedSets:
                url = 'https://api.forecast.io/forecast/{0}/{1},{2},{3}?units=si'.format(get_api_key(), current_longitude, current_latitude, current_time)
                print url
                data = json.load(urllib2.urlopen(url, timeout=1.5))

                hoursArray = data["hourly"]["data"]

                for hourdata in hoursArray:
                    hourdata["longitude"] = current_longitude
                    hourdata["latitude"] = current_latitude

                #next day
                current_time+=86400
                globallyDownloadedSets = currentlyDownloadedDataSets+1

                #speichern in JSON config
                weather_config["meta"]["downloadedSets"] = globallyDownloadedSets
                weather_config["data"] = weather_config["data"] + hoursArray
                d.write_json_file(weather_config,'weather-data-collection.json')
            currentlyDownloadedDataSets+=1
        current_latitude+=1
    current_longitude-=1

print "Done."
