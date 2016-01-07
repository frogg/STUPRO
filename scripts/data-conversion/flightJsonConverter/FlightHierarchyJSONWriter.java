import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map.Entry;

import com.opencsv.CSVReader;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;

public class FlightHierarchyJSONWriter {
	static HashMap<Integer, String[]> zoom1 = new HashMap<Integer, String[]>();
	static HashMap<Integer, String[]> zoom2 = new HashMap<Integer, String[]>();
	static HashMap<Integer, String[]> zoom3 = new HashMap<Integer, String[]>();
	static HashMap<Integer, String[]> zoom4 = new HashMap<Integer, String[]>();
	static HashMap<Integer, String[]> zoom5 = new HashMap<Integer, String[]>();

	public static void main(String[] args) throws IOException {
		try {
			CSVReader reader = new CSVReader(new FileReader(
				"FlightroutesJava.csv"));
			String[] nextLine;
			while ((nextLine = reader.readNext()) != null) {

				double distance = calculateDistance(
						Double.parseDouble(nextLine[1]),
						Double.parseDouble(nextLine[2]),
						Double.parseDouble(nextLine[3]),
						Double.parseDouble(nextLine[4]));
				if (distance >= 5000) {
					zoom1.put(Integer.parseInt(nextLine[0]), nextLine);
				} else if (distance >= 3000 && distance < 5000) {
					zoom2.put(Integer.parseInt(nextLine[0]), nextLine);
				} else if (distance >= 1500 && distance < 3000) {
					zoom3.put(Integer.parseInt(nextLine[0]), nextLine);
				} else if (distance >= 500 && distance < 1500) {
					zoom4.put(Integer.parseInt(nextLine[0]), nextLine);
				} else if (distance >= 0 && distance < 500) {
					zoom5.put(Integer.parseInt(nextLine[0]), nextLine);
				}
			}
			reader.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}

		defineParent(zoom2, zoom1);
		defineParent(zoom3, zoom2);
		defineParent(zoom4, zoom3);
		defineParent(zoom5, zoom4);

		JSONObject flightJSON = new JSONObject();
		JSONObject meta = new JSONObject();
		JSONObject root = new JSONObject();
		meta.put("temporal", new Boolean(false));
		meta.put("dataType", new String("flights"));
		flightJSON.put("meta", meta);

		JSONArray level1 = new JSONArray();

		for (Entry<Integer, String[]> entry1 : zoom1.entrySet()) {
			JSONObject flightObject1 = new JSONObject();
			JSONObject coordinatesSource1 = new JSONObject();
			JSONObject coordinatesDestination1 = new JSONObject();
			coordinatesSource1.put("latitude",
					new Double(Double.parseDouble(entry1.getValue()[1])));
			coordinatesSource1.put("longitude",
					new Double(Double.parseDouble(entry1.getValue()[2])));
			coordinatesDestination1.put("latitude",
					new Double(Double.parseDouble(entry1.getValue()[3])));
			coordinatesDestination1.put("longitude",
					new Double(Double.parseDouble(entry1.getValue()[4])));

			flightObject1.put("startposition", coordinatesSource1);
			flightObject1.put("endposition", coordinatesDestination1);
			// -------------------------------------------------------------------------------------------------
			JSONArray level2 = new JSONArray();
			for (Entry<Integer, String[]> entry2 : zoom2.entrySet()) {
				if (entry2.getValue()[6].equals(entry1.getValue()[0])) {
					JSONObject flightObject2 = new JSONObject();
					JSONObject coordinatesSource2 = new JSONObject();
					JSONObject coordinatesDestination2 = new JSONObject();
					coordinatesSource2
							.put("latitude",
									new Double(Double.parseDouble(entry2
											.getValue()[1])));
					coordinatesSource2
							.put("longitude",
									new Double(Double.parseDouble(entry2
											.getValue()[2])));
					coordinatesDestination2
							.put("latitude",
									new Double(Double.parseDouble(entry2
											.getValue()[3])));
					coordinatesDestination2
							.put("longitude",
									new Double(Double.parseDouble(entry2
											.getValue()[4])));

					flightObject2.put("startposition", coordinatesSource2);
					flightObject2.put("endposition", coordinatesDestination2);
					// -------------------------------------------------------------------------------------------------
					JSONArray level3 = new JSONArray();
					for (Entry<Integer, String[]> entry3 : zoom3.entrySet()) {
						if (entry3.getValue()[6].equals(entry2.getValue()[0])) {
							JSONObject flightObject3 = new JSONObject();
							JSONObject coordinatesSource3 = new JSONObject();
							JSONObject coordinatesDestination3 = new JSONObject();
							coordinatesSource3.put("latitude", new Double(
									Double.parseDouble(entry3.getValue()[1])));
							coordinatesSource3.put("longitude", new Double(
									Double.parseDouble(entry3.getValue()[2])));
							coordinatesDestination3.put("latitude", new Double(
									Double.parseDouble(entry3.getValue()[3])));
							coordinatesDestination3.put(
									"longitude",
									new Double(Double.parseDouble(entry3
											.getValue()[4])));

							flightObject3.put("startposition",
									coordinatesSource3);
							flightObject3.put("endposition",
									coordinatesDestination3);
							// -------------------------------------------------------------------------------------------------
							JSONArray level4 = new JSONArray();
							for (Entry<Integer, String[]> entry4 : zoom4
									.entrySet()) {
								if (entry4.getValue()[6].equals(entry3
										.getValue()[0])) {
									JSONObject flightObject4 = new JSONObject();
									JSONObject coordinatesSource4 = new JSONObject();
									JSONObject coordinatesDestination4 = new JSONObject();
									coordinatesSource4.put(
											"latitude",
											new Double(Double
													.parseDouble(entry4
															.getValue()[1])));
									coordinatesSource4.put(
											"longitude",
											new Double(Double
													.parseDouble(entry4
															.getValue()[2])));
									coordinatesDestination4.put(
											"latitude",
											new Double(Double
													.parseDouble(entry4
															.getValue()[3])));
									coordinatesDestination4.put(
											"longitude",
											new Double(Double
													.parseDouble(entry4
															.getValue()[4])));

									flightObject4.put("startposition",
											coordinatesSource4);
									flightObject4.put("endposition",
											coordinatesDestination4);
									// -------------------------------------------------------------------------------------------------
									JSONArray level5 = new JSONArray();
									for (Entry<Integer, String[]> entry5 : zoom5
											.entrySet()) {
										if (entry5.getValue()[6].equals(entry4
												.getValue()[0])) {
											JSONObject flightObject5 = new JSONObject();
											JSONObject coordinatesSource5 = new JSONObject();
											JSONObject coordinatesDestination5 = new JSONObject();
											coordinatesSource5
													.put("latitude",
															new Double(
																	Double.parseDouble(entry5
																			.getValue()[1])));
											coordinatesSource5
													.put("longitude",
															new Double(
																	Double.parseDouble(entry5
																			.getValue()[2])));
											coordinatesDestination5
													.put("latitude",
															new Double(
																	Double.parseDouble(entry5
																			.getValue()[3])));
											coordinatesDestination5
													.put("longitude",
															new Double(
																	Double.parseDouble(entry5
																			.getValue()[4])));

											flightObject5.put("startposition",
													coordinatesSource5);
											flightObject5.put("endposition",
													coordinatesDestination5);

											level5.add(flightObject5);
										}
									}
									flightObject4.put("children", level5);
									// -------------------------------------------------------------------------------------------------
									level4.add(flightObject4);
								}
							}
							flightObject3.put("children", level4);
							// -------------------------------------------------------------------------------------------------
							level3.add(flightObject3);
						}
					}
					flightObject2.put("children", level3);
					// -------------------------------------------------------------------------------------------------
					level2.add(flightObject2);
				}
			}
			flightObject1.put("children", level2);
			// -------------------------------------------------------------------------------------------------
			level1.add(flightObject1);

		}
		root.put("children", level1);
		flightJSON.put("root", root);
		writeJSONFile(flightJSON);
	}

	public static int checkMinDistance(HashMap<Integer, String[]> zoomLevel,
			double lat, double lon) {
		int minIndex = 0;
		double minDistance = Double.MAX_VALUE;
		for (Entry<Integer, String[]> entry : zoomLevel.entrySet()) {
			double distance = calculateDistance(lat, lon,
					Double.parseDouble(entry.getValue()[1]),
					Double.parseDouble(entry.getValue()[2]));
			if (distance < minDistance) {
				minDistance = distance;
				minIndex = Integer.parseInt(entry.getValue()[0]);
			}
		}

		return minIndex;
	}

	public static double calculateDistance(double lat1, double lon1,
			double lat2, double lon2) {
		double lat = ((lat1 + lat2) / 2 * (Math.PI / 180.0));
		double widthDegreeLat = 111.3;
		double widthDegreeLon = widthDegreeLat * Math.cos(lat);
		double dx = widthDegreeLon * (lon1 - lon2);
		double dy = widthDegreeLat * (lat1 - lat2);
		// checks if fligth is over map-border and subtracts
		// the distance from the earths circumference (approx. 40074km)
		double rawDistance = Math.sqrt(dx * dx + dy * dy);
		if (rawDistance > 20037) {
			return 40074 - rawDistance;
		} else {
			return rawDistance;
		}

	}

	public static void defineParent(HashMap<Integer, String[]> childMap,
			HashMap<Integer, String[]> parentMap) {
		for (Entry<Integer, String[]> entry : childMap.entrySet()) {
			int parentIndex = checkMinDistance(parentMap,
					Double.parseDouble(entry.getValue()[1]),
					Double.parseDouble(entry.getValue()[2]));
			entry.getValue()[6] = Integer.toString(parentIndex);
		}
	}

	public static void writeJSONFile(JSONObject flightsObject) {
		try {
			File jsonFile = new File("flights.json");
			FileWriter writer = new FileWriter(jsonFile);
			writer.write(flightsObject.toJSONString());
			writer.flush();
			writer.close();
			System.out.println("File succesfully converted");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
