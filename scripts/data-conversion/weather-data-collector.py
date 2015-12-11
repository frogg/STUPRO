import os.path
import json
import urllib2
import requests

from DataUtility import DataUtility

config_file_name = 'forecastio.config'

def get_api_key():
    if os.path.isfile(config_file_name) and len(open('forecastio.config', 'rb').read().splitlines()) > 0:
        return open('forecastio.config', 'rb').read().splitlines()[0]
    else:
        raise Exception('The configuration file \'{0}\' containing your API key is invalid or does not exist.'.format(config_file_name))

# url = 'https://api.forecast.io/forecast/{0}/{1},{2},{3}?units=si'.format(get_api_key(), 48.771904, 9.182626, 1438441200)
# data = json.dumps(json.load(urllib2.urlopen(url, timeout=0.2)), indent=4)
d = DataUtility()

print get_api_key()
print d.read_json_file('weather-data-collection.json')
