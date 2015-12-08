import csv
import xmltodict
import json

class DataUtility:
    def read_csv_file(self, filename):
        """
        Read the contents of a CSV file and return it as a list of dictionaries using the keys
        specified in the first line of the file.
        """
        with open(filename, 'rb') as csv_file:
            return [row for row in csv.DictReader(csv_file, delimiter=',', quotechar='"')]

    def read_xml_file(self, filename):
        """
        Read the contents of an XML file and return it as a nested dictionary.
        """
        return xmltodict.parse(open(filename, 'rb').read(), attr_prefix='')

    def read_json_file(self, filename):
        """
        Read the contents of a JSON file and return it as a nested dictionary.
        """
        with open(filename) as json_file:
            return json.load(json_file)

    def write_json_file(self, data, filename):
        """
        Write some data stored in a dictionary as a new JSON file.
        """
        open(filename, 'w').write(json.dumps(data, indent=4))
