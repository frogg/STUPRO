#include "Utils/Database/CitiesDatabase.hpp"

#include <iostream>

using namespace std;
using namespace pqxx;

void CitiesDatabase::createCitiesTable(){
    cout << "createCitiesTable";

    const char *sql = "CREATE TABLE IF NOT EXISTS CITY("  \
    "ID SERIAL PRIMARY KEY ," \
    "NAME           TEXT    NOT NULL," \
    "COUNTRYCODES CHAR(2)," \
    "LATITUDE NUMERIC(6, 4)," \
    "LONGITUDE NUMERIC(6, 4));";
    work W(*this->dbConnection);
    W.exec( sql );
    W.commit();
    cout << "Table created successfully or exits already" << endl;
}

void CitiesDatabase::insertOperation(){
    cout << "tsadkfjaInsertOperation";
    const char *sql = "INSERT INTO CITY (NAME,COUNTRYCODES,LATITUDE,LONGITUDE) "  \
    "VALUES ('Berlin','DE',25.04992,25.04992); ";
    /* Create a transactional object. */
    work W(*this->dbConnection);
    
    /* Execute SQL query */
    W.exec( sql );
    W.commit();
    cout << "Records created successfully" << endl;
}

void CitiesDatabase::getAllCities(){
    cout << "createCitiesTable";

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
    cout << "Operation done successfully" << endl;
}
void CitiesDatabase::getCity(std::string name, std::vector<City> *cities){
    
    cout << "getCity";
    
    /* Create SQL statement */
    string sql = "SELECT * from CITY where NAME = '"+ name +"'";
    
    /* Create a non-transactional object. */
    nontransaction N(*this->dbConnection);
    cout << "getCity1";

    /* Execute SQL query */
    result R( N.exec( sql ));
    cout << "getCity2";

    /* List down all the records */
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        
        City result;
        result.name = c[1].as<string>();
        result.countryCode = c[2].as<string>();
        result.latitude = c[3].as<float>();
        result.longitude = c[4].as<float>();
        
        cities->push_back(result);
        cout << "Name = " << c[1].as<string>() << endl;
        cout << "COUNTRYCODES = " << c[2].as<string>() << endl;
        cout << "LATITUDE = " << c[3].as<float>() << endl;
        cout << "LONGITUDE = " << c[4].as<float>() << endl;
    }
    cout << "Operation done successfully, getCity" << endl;

}
