"""
skript uses python 3.5
"""
import json
import sys
import csv
import math
from DataUtility import DataUtility

def main(argv):

    zoom1 = []
    zoom2 = []
    zoom3 = []
    zoom4 = []
    zoom5 = []
	
    d = DataUtility()
    csv_data = d.read_csv_file('Flightroutes.csv')
    #fills the 5 zoomlists which represents the priority-levels 
    for row in csv_data:
        distance = calculateDistance(float(row["srcLatitude"]), float(row["srcLongitude"]),float(row["destLatitude"]),float(row["destLongitude"]))
        flight = [int(row["id"]),float(row["srcLatitude"]),float(row["srcLongitude"]),float(row["destLatitude"]),float(row["destLongitude"]),int(row["parent"])]
        if distance >= 5000:       
            zoom1.append(flight)
        elif(distance < 5000 and distance >= 3000):
            zoom2.append(flight)
        elif(distance < 3000 and distance >= 1500):
            zoom3.append(flight)
        elif(distance < 1500 and distance >= 500):
            zoom4.append(flight)
        elif(distance < 500):
            zoom5.append(flight)
    #defines the child-parent relationship for every flight 
    defineParent(zoom2, zoom1)
    defineParent(zoom3, zoom2)
    defineParent(zoom4, zoom3)
    defineParent(zoom5, zoom4)
    counter = 0
    level1Entries = []
    for entry1 in zoom1:
#################################################### start level 2
        level2Entries = []
        for entry2 in zoom2:
#################################################### start level 3            
            level3Entries = []
            for entry3 in zoom3:
#################################################### start level 4                            
                level4Entries = []
                for entry4 in zoom4:
#################################################### start level 5                            
                    level5Entries = []
                    for entry5 in zoom5:
                        if entry4[0] == entry5[5]:          
                            flight = {'startPosition':{'latitude':entry5[1],'longitude':entry5[2]},'endPosition':{'latitude':entry5[3],'longitude':entry5[4]}} 
                            level5Entries.append(flight)
#################################################### end level 5  
                    if entry3[0] == entry4[5]:          
                        flight = {'startPosition':{'latitude':entry4[1],'longitude':entry4[2]},'endPosition':{'latitude':entry4[3],'longitude':entry4[4]},'children':level5Entries} 
                        level4Entries.append(flight)
#################################################### end level 4                 
                if entry2[0] == entry3[5]:
                    flight = {'startPosition':{'latitude':entry3[1],'longitude':entry3[2]},'endPosition':{'latitude':entry3[3],'longitude':entry3[4]},'children':level4Entries}
                    level3Entries.append(flight)   
#################################################### end level 3            
            if entry1[0] == entry2[5]:
                flight = {'startPosition':{'latitude':entry2[1],'longitude':entry2[2]},'endPosition':{'latitude':entry2[3],'longitude':entry2[4]},'children':level3Entries}            
                level2Entries.append(flight)
#################################################### end level 2       
        flight = {'startPosition':{'latitude':entry1[1],'longitude':entry1[2]},'endPosition':{'latitude':entry1[3],'longitude':entry1[4]},'children':level2Entries}
        level1Entries.append(flight)
        level1Entries.append(flight)

    flights = {'meta': {'dataType': 'flights', 'temporal': False}, 'root': {'children': level1Entries}}	
    d.write_json_file(flights,'flights.json', True)
	
"""
defines the hierarchical parent of a flight and writes it in to 
the flights array 
"""
def defineParent(childList, parentList):
    for entry in childList:
        parentIndex = checkMindistance(parentList, entry[1], entry[2])
        entry[5] = parentIndex
		
"""
approximation of distance betwenn two coordinates in kilometers.
an approximation is enough in this case, because it only results 
in one of five priority-levels
"""
def calculateDistance(lat1, lon1, lat2, lon2):
    lat = ((lat1 + lat2) / (math.pi/180))
    widthDegreeLat = 111.3
    widthDegreeLon = widthDegreeLat * math.cos(lat)
    dx = widthDegreeLon * (lon1 - lon2)
    dy = widthDegreeLat * (lat1 - lat2)
    # checks if fligth is over map-border and subtracts
    # the distance from the earths circumference (approx. 40074km)
    rawdistance = math.sqrt(dx * dx + dy * dy)
    if (rawdistance > 20037):
        return 40074-rawdistance
    else:
        return rawdistance	
	
"""
returns the index of the parentFlight, which has the minimum distance between
the source-airports
"""
def checkMindistance(zoomlevel, lat, lon):
    minIndex = 0
    minDistance = 50000
    for entry in zoomlevel:
        distance = calculateDistance(lat, lon, entry[1], entry[2])
        if distance < minDistance:
            minDistance = distance
            minIndex = entry[0]
    return minIndex
		
if __name__ == "__main__":
    main(sys.argv[1:])