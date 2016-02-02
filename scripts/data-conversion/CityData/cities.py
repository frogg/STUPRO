from DataUtility import DataUtility
from collections import OrderedDict

count = [0]

def reformat_dictionary(d):
    if 'children' not in d and 'root' not in d:
        d['children'] = []
    for k, v in d.iteritems():
        if isinstance(v, OrderedDict):
            if k == 'children':
                d[k] = [v]
            reformat_dictionary(v)
        elif isinstance(v, list):
            for e in v:
                reformat_dictionary(e)
        else:
            if k == 'longitude' or k == 'latitude':
                d[k] = float(v)
            if k == 'name':
                count[0] += 1
                if count[0] % 100 == 0:
                    print "Formatting city {0}".format(count[0])

d = DataUtility()

print 'Reading the XML file...'

xml_content = d.read_xml_file('cities.xml')
reformat_dictionary(xml_content)
xml_content = xml_content['root']

cities = {'meta': {'dataType': 'cities', 'temporal': False}, 'root': xml_content}

print 'Writing the JSON file...'

d.write_json_file(cities, 'cities.json', True)
