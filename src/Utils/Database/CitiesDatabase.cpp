#include "Utils/Database/CitiesDatabase.hpp"

#include <iostream>

using namespace std;
using namespace pqxx;

void CitiesDatabase::createCitiesTable(){
    const char *sql = "CREATE TABLE IF NOT EXISTS CITIES("  \
    "ID INT PRIMARY KEY     NOT NULL AUTO_INCREMENT," \
    "NAME           TEXT    NOT NULL," \
    "COUNTRYCODES VARCHAR(10)," \
    "LONGITUDE DECIMAL(2,6)," \
    "LATITUDE  DECIMAL(2,6));";
    work W(*this->dbConnection);
    W.exec( sql );
    W.commit();
    cout << "Table created successfully or exits already" << endl;
}

void CitiesDatabase::insertOperation(){
    const char *sql = "INSERT INTO CITIES (NAME,COUNTRYCODES,LONGITUDE,LATITUDE) "  \
    "VALUES ('Berlin', 'ED', 40.765278, 40.765278 );";
    /* Create a transactional object. */
    work W(*this->dbConnection);
    
    /* Execute SQL query */
    W.exec( sql );
    W.commit();
    cout << "Records created successfully" << endl;
}

//void CitiesDatabase::getCity(){
//    /* Create SQL statement */
//    const char* sql = "SELECT * from CITIES";
//    
//    /* Create a non-transactional object. */
//    nontransaction N(*this->dbConnection);
//    
//    /* Execute SQL query */
//    result R( N.exec( sql ));
//    
//    /* List down all the records */
//    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
//        cout << "ID = " << c[0].as<int>() << endl;
//        cout << "Name = " << c[1].as<string>() << endl;
//        cout << "Age = " << c[2].as<int>() << endl;
//        cout << "Address = " << c[3].as<string>() << endl;
//        cout << "Salary = " << c[4].as<float>() << endl;
//    }
//    cout << "Operation done successfully" << endl;
//}
//
