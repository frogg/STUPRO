from twitter import *
import html
import cgi
from DataUtility import DataUtility

import sys
import csv
import random

#Stuttgart
latitude = 48.778283	# geographical centre of search
longitude = 9.176183	# geographical centre of search

max_range = 1000 		# search range in kilometres
num_results = 100		# minimum results to obtain

outfile = "output.csv"

#load credentials from config
config = {}
execfile("tweet_config.py", config)


twitter = Twitter(auth = OAuth(config["access_key"], config["access_secret"], config["consumer_key"], config["consumer_secret"]))


csvfile = open(outfile, 'a')
csvreader = csv.reader(csvfile,quoting=csv.QUOTE_ALL)
csvwriter = csv.writer(csvfile,quoting=csv.QUOTE_ALL)

#row = ["tweet-id", "user", "text", "latitude", "longitude", "timestamp"]
#csvwriter.writerow(row)

#bis zu 100 Suchresultate pro Aufruf
result_count = 0
last_id = None
while result_count <  num_results:
	
	longitude  = random.randrange(-180, 180)
	latitude = random.randrange(-90, 90)
	
	query = twitter.search.tweets(q = "", geocode = "%f,%f,%dkm" % (latitude, longitude, max_range), count = 100, max_id = last_id)

	for result in query["statuses"]:
		
		#result contains geodata
		if result["geo"]:
			user = result["user"]["screen_name"]
			text = result["text"]
			text = text.encode('utf-8', 'replace')
			text = text.replace('\n',' ')
#			text = cgi.escape(text).encode('ascii','xmlcharrefreplace')
			

			latitude = result["geo"]["coordinates"][0]
			longitude = result["geo"]["coordinates"][1]
			tweetid = result["id"]
			created_at = result["created_at"]

			#write in csv
			row = [tweetid, user, text, latitude, longitude, created_at]
			
			csvwriter.writerow(row)
			result_count += 1
		last_id = result["id"]


	print "got %d results" % result_count


csvfile.close()

print "written to %s" % outfile

d = DataUtility()

print d.read_csv_file(outfile)

