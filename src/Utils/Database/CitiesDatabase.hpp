#ifndef KRONOS_UTILS_DATABSE_CITIESDATABASE
#define KRONOS_UTILS_DATABSE_CITIESDATABASE

#include <Utils/Database/PostgresDB.hpp>
#include <Utils/Misc/City.hpp>
#include <QSqlQuery>
#include <QList>

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
	CitiesDatabase (const QString databaseName, const QString user,
	                const QString password, const QString hostaddr,
	                int port);
	/**
	 * get Cities from a cityname
	 * @param name, name of the city we search
	 @ @param reference to vector of cities which will be filled in function (containing lat-long information) with the given name
	 */
	QList<City> getCity(QString name);

private:
	QSqlQuery query;
	bool isPrepared;
};

#endif
