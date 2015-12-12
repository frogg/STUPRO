#ifndef KRONOS_READER_DATATYPE_HPP
#define KRONOS_READER_DATATYPE_HPP

class DataType {
public:
    /**
     * Constant to be used if the data represents a set of cities.
     */
    const static int CITIES = 0;
    /**
     * Constant to be used if the data represents a set of flights.
     */
    const static int FLIGHTS = 1;
    /**
     * Constant to be used if the data represents a set of tweets.
     */
    const static int TWEETS = 2;
    /**
     * Constant to be used if the data represents a set of precipitation data points.
     */
    const static int PRECIPITATION = 3;
    /**
     * Constant to be used if the data represents a set of temperature data points.
     */
    const static int TEMPERATURE = 4;
    /**
     * Constant to be used if the data represents a set of wind data points.
     */
    const static int WIND = 5;
};

#endif
