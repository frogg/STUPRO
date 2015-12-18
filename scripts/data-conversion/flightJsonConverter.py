import json
import sys
import csv
from DataUtility import DataUtility

def main(argv):
    d = DataUtility()
    csv_data = d.read_csv_file('dummy-data/Flightroutes.csv')
    
    f = []
    for row in csv_data:
        data = {'distance':row["distance"],'startPosition':{'latitude':row["srcLatitude"],'longitude':row["srcLongitude"]},'endPosition':{'latitude':row["destLatitude"],'longitude':row["destLongitude"]}}
        f.append(data)
    flights = {'meta': {'dataType': 'flights', 'temporal': False}, 'root': f}	
    d.write_json_file(flights,'test-data/flights.json', True)
    #with open('flights1.json', 'w') as fp:
    #    json.dump(csv_data, fp)
    
if __name__ == "__main__":
    main(sys.argv[1:])