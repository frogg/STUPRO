from DataUtility import DataUtility
from collections import OrderedDict

from sys import maxint, stdout
from math import radians, cos, sin, asin, sqrt

maximal_zoom_level = 10

def distance(latitude_1, longitude_1, latitude_2, longitude_2):
    """
    Calculate the great circle distance between two points 
    on the earth (specified in decimal degrees)
    """
    longitude_1, latitude_1, longitude_2, latitude_2 = map(radians, [longitude_1, latitude_1, longitude_2, latitude_2])
    
    dlon = longitude_2 - longitude_1 
    dlat = latitude_2 - latitude_1 
    a = sin(dlat/2)**2 + cos(latitude_1) * cos(latitude_2) * sin(dlon/2)**2
    c = 2 * asin(sqrt(a)) 
    km = 6367 * c
    return km
    
def split_list(list, size):
    """
    Split a list into around equally sized pieces
    """
    newseq = []
    splitsize = 1.0/size*len(list)
    for i in range(size):
        newseq.append(list[int(round(i*splitsize)):int(round((i+1)*splitsize))])
    return newseq
    
d = DataUtility()

print 'Reading the XML file...'

csv_content = d.read_csv_file('flightJsonConverter/Flightroutes.csv')
print 'Read {0} lines of data.'.format(len(csv_content))

all_flights = []
for index, item in enumerate(csv_content):
    all_flights.append(
        {
            'startPosition': {
                'longitude': float(item['srcLongitude']),
                'latitude': float(item['srcLatitude'])
            },
            'endPosition': {
                'longitude': float(item['destLongitude']),
                'latitude': float(item['destLatitude'])
            },
            'length': distance(
                float(item['srcLatitude']), float(item['srcLongitude']),
                float(item['destLatitude']), float(item['destLongitude'])
            ),
            'children': []
        }
    )

sorted_flights = sorted(all_flights, key = lambda k: k['length'])
segmented_flights = split_list(sorted_flights, maximal_zoom_level)

print '\nSegmenting flights into groups based on flight length...'

for segment in segmented_flights:
    print 'Created segment with {0} entries (flight length {1} - {2} km)'.format(len(segment), segment[0]['length'], segment[len(segment) - 1]['length'])
    for flight in segment:
        flight.pop('length', None)

print '\nCreating tree from flight data...\n'

tree = segmented_flights[0]

for index in range(1, maximal_zoom_level - 1):
    print 'Arranging zoom level {0}/{1}...\n'.format(index, maximal_zoom_level - 1)
    new_tree = segmented_flights[index]
    current_index = 0
    for item in tree:
        best_parent_index = {}
        current_distance = maxint
        for parent_index, parent in enumerate(new_tree):
            parent_distance = distance(
                item['startPosition']['latitude'], item['startPosition']['longitude'],
                parent['startPosition']['latitude'], parent['startPosition']['longitude']
            )
            
            if (parent_distance < current_distance):
                best_parent_index = parent_index
                current_distance = parent_distance
        
        new_tree[best_parent_index]['children'].append(item)
                
        if current_index % 20 == 0 or current_index + 1 == len(tree):
            stdout.write('\r' + str('Item {0}/{1}'.format(current_index, len(tree) - 1)))
            stdout.flush()
        
        current_index += 1
    tree = new_tree
    print '\n'

print 'Writing rearranged data to JSON file...'

flights = {'meta': {'dataType': 'flights', 'temporal': False}, 'root': { 'children': tree}}
d.write_json_file(flights, 'test-data/flights.json', True)
