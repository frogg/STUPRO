from DataUtility import DataUtility

d = DataUtility()

# Read an example CSV file
csv_file = d.read_csv_file('example-data/test.csv')
print csv_file

# Access a specific row and key
print csv_file[0]['author']

# Read an example XML file
xml_file = d.read_xml_file('example-data/test.xml')
print xml_file

# Access a specific entry from the XML tree
print xml_file['cities']['stadt']['stadt'][1]['name']

# Read an example JSON file
json_file = d.read_json_file('example-data/test.json')
print json_file

# Access a specific entry from the JSON tree
print json_file['globe']['radius']

# Write one of the dictionaries to a new JSON file
d.write_json_file(csv_file, 'example-data/output.json')
