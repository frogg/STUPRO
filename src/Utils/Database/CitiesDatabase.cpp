#include "Utils/Database/CitiesDatabase.hpp"

#include <iostream>

using namespace std;
using namespace pqxx;


void CitiesDatabase::printAllCities() {

	/* Create SQL statement */
	const char* sql = "SELECT * from CITY";

	/* Create a non-transactional object. */
	nontransaction N(*this->dbConnection);

	/* Execute SQL query */
	result R( N.exec( sql ));

	/* List down all the records */
	for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
		cout << "ID = " << c[0].as<int>() << endl;
		cout << "Name = " << c[1].as<string>() << endl;
		cout << "COUNTRYCODES = " << c[2].as<string>() << endl;
		cout << "LATITUDE = " << c[3].as<float>() << endl;
		cout << "LONGITUDE = " << c[4].as<float>() << endl;
	}
}

void CitiesDatabase::getCity(std::string name, std::vector<City>* cities) {
	try {
		/* Create SQL statement */
		string sql = "SELECT * from CITY where LOWER(NAME) like LOWER('%" + name +
		             "%') ORDER BY CHAR_LENGTH(NAME), NAME DESC;";

		/* Create a non-transactional object. */
		nontransaction N(*this->dbConnection);

		/* Execute SQL query */
		result R( N.exec( sql ));

		/* List down all the records */
		for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
			City result;
			result.name = c[1].as<string>();
			result.countryCode = c[2].as<string>();
			result.latitude = c[3].as<float>();
			result.longitude = c[4].as<float>();
			//store result in vector
			cities->push_back(result);
		}
		//cout << "Operation done successfully, getCity" << endl;
	} catch (pqxx::data_exception e) {
		cout << "Error executing SQL query:" << std::endl << e.what() << std::endl;
	}
}
