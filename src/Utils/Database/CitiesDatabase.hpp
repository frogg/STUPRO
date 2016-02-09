#ifndef KRONOS_UTILS_DATABSE_CITIESDATABASE
#define KRONOS_UTILS_DATABSE_CITIESDATABASE

#include <pqxx/pqxx>
#include <Utils/Database/PostgresDB.hpp>
#include <Utils/Misc/City.hpp>
#include <vector>
#include <string>

using namespace std;
using namespace pqxx;

/**
 * PostgresDatabase storing the city information
 */
class CitiesDatabase : public PostgresDB {

public:
	/**
	 * create database connection, with the given parameters
	 * @param databaseName
	 * @param user database user, make sure that you executed the script (in scripts) creating them
	 * @param hostadr address where the database can be accessed
	 * @param post post where the database can be accessed
	 */
	CitiesDatabase (const std::string& databaseName, const std::string& user,
	                const std::string& password, const std::string& hostaddr,
	                const std::string& port) : PostgresDB(databaseName, user, password, hostaddr, port) {

	};
	/**
	 * get Cities from a cityname
	 * @param name, name of the city we search
	 @ @param reference to vector of cities which will be filled in function (containing lat-long information) with the given name
	 */
	void getCity(std::string name, std::vector<City>* cities);
	/**
	 *print all cities that exist in your database on standard output
	 */
	void printAllCities();

};

#endif
