#ifndef STUPRO_CITY_HPP
#define STUPRO_CITY_HPP

#include <string>


class City {
public:
    City();
    std::string name;
    std::string countryCode;
    float latitude;
    float longitude;
};

#endif