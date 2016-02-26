import json
from DataUtility import DataUtility
import datetime
import time


d = DataUtility()

twitterjsondata = d.read_tabbed_file("Rohdaten_Sandy/numeric_20121022_clean.csv")

tweets = []


for tweet in twitterjsondata:
	
	try:

		tweet["longitude"] = float(tweet["lon"])
		tweet["latitude"] = float(tweet["lat"])
		tweet["children"] = []
		tweet["timestamp"] = 0
		tweet["numberOfRetweets"] = 10
		#timestamp = tweet["timestamp"]

		#tweet["timestamp"] = int(time.mktime(datetime.datetime.strptime(str(timestamp), "%Y%m%d%H%M%S").timetuple()))

		#print tweet
		tweets.append(tweet)
		

	except KeyError as e:
		print "error" + str(e)

newtwitterjsondata = {'meta': {'dataType': 'tweets', 'temporal': True, 'timeResolution': 3600}, 'root': {'children': tweets}}

d.write_json_file(newtwitterjsondata, "tweets.kJson", True)
