#ifndef KRONOS_READER_DATA_HPP
#define KRONOS_READER_DATA_HPP

class Data {
public:
	/**
	 * Enum that denotes a data type. Do not assign values to these.
	 */
	enum Type {
		CITIES, FLIGHTS, TWEETS, PRECIPITATION, TEMPERATURE, WIND, CLOUD_COVERAGE
	};

	/**
	 * Check whether a data type contains time information.
	 * @param dataType Data type to be checked
	 * @return True if the data type contains time information, false otherwise
	 */
	static bool isTemporal(const Data::Type dataType);
};

#endif
