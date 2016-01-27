#ifndef KRONOS_READER_DATA_HPP
#define KRONOS_READER_DATA_HPP

class Data {
public:
    /**
     * Constant to be used if the data represents a set of cities.
     */
    // const static int CITIES = 0;
    /**
     * Constant to be used if the data represents a set of flights.
     */
    // const static int FLIGHTS = 1;
    /**
     * Constant to be used if the data represents a set of tweets.
     */
    // const static int TWEETS = 2;
    /**
     * Constant to be used if the data represents a set of precipitation data points.
     */
    // const static int PRECIPITATION = 3;
    /**
     * Constant to be used if the data represents a set of temperature data points.
     */
    // const static int TEMPERATURE = 4;
    /**
     * Constant to be used if the data represents a set of wind data points.
     */
    // const static int WIND = 5;
	/**
	* Constant to be used if the data represents a set of cloudCover data points.
	*/
	// const static int CLOUDCOVER = 6;
    
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
