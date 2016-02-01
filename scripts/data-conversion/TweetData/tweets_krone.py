import json
from DataUtility import DataUtility
import datetime
import time


d = DataUtility()

twitterjsondata = d.read_tabbed_file("Sandy Rohdaten/result.csv")

tweets = []


for tweet in twitterjsondata:
	print tweet
	try:

		tweet["longitude"] = float(tweet["longitude"])
	    tweet["latitude"] = float(tweet["latitude"])
	    tweet["children"] = []
        tweet["timestamp"] = 0
	    #timestamp = tweet["timestamp"]

    	#tweet["timestamp"] = int(time.mktime(datetime.datetime.strptime(str(timestamp), "%Y%m%d%H%M%S").timetuple()))


	    tweets.append(tweet)

	except KeyError as e:
		print e

newtwitterjsondata = {'meta': {'dataType': 'tweets', 'temporal': True, 'timeResolution': 3600}, 'root': {'children': tweets}}

d.write_json_file(newtwitterjsondata, "tweets.kJson", True)
