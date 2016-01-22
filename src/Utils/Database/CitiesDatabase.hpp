#include <pqxx/pqxx>
#include <Utils/Database/PostgresDB.hpp>
#include "Utils/City.hpp"
#include <vector>
#include <string>

using namespace std;
using namespace pqxx;

class CitiesDatabase : public PostgresDB {
public:
    CitiesDatabase (const std::string &databaseName, const std::string &user, const std::string &password, const std::string &hostaddr, const std::string &port) : PostgresDB(databaseName,user,password, hostaddr, port){
        
    };
    void createCitiesTable();
    void insertOperation();
    void getAllCities();
    void getCity(std::string name, std::vector<City> *cities);
  
};





