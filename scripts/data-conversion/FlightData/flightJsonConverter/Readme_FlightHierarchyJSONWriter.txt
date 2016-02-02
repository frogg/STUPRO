initializing the .csv-File:
The initial Flightdata.csv (or the FlightdataJava.csv) has to be brought to 
the following column-format:
id, srcLatitude, srcLongitude, destLatitude, destLongitude, distance, Parent 
where Parent is initialized with 0.
The .csv-File for the java-converter starts with the first data-row and not 
with the header-line, while the python-version has to use the above named headers.



converting to JSON:
there are two converter available, written in python or java, which do the same, but
the java-version is much faster.
The FlightHierarchyJSONWriter.java needs the two following libraries to work:
- opencsv-3.6.jar
- jaon-simple-1.1.1.jar

The resulting flights.json-File has the following format:

root: {"children":[																		
			{"startPosition":{
							"latitude":"9.55",
							"longitude":"100.06"
							},
			"endPosition":{"latitude":"8.10",
							"longitude":"98.99"
							},
			"children":[															
						{"startPosition":{
										"latitude":"9.55",
										"longitude":"100.06"
										},
						"endPosition":{
										"latitude":"8.10",
										"longitude":"98.99"
										}},
						children:{   ....											
						}
			}				},
			{"startPosition":{
							"latitude":"9.55",
							"longitude":"100.06"
							},
			"endPosition":{"latitude":"8.10",
							"longitude":"98.99"
							}
			children:{															
						{"startPosition":{
										"latitude":"9.55",
										"longitude":"100.06"
										},
						"endPosition":{
										"latitude":"8.10",
										"longitude":"98.99"
										}},
						children:{   ....											
						}
			}},
			{...
							}: