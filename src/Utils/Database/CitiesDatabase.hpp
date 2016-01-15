#include <pqxx/pqxx>
#include <Utils/Database/PostgresDB.hpp>

using namespace std;
using namespace pqxx;

class CitiesDatabase : public PostgresDB {
public:
    CitiesDatabase (const std::string &databaseName, const std::string &user, const std::string &password, const std::string &hostaddr, const std::string &port) : PostgresDB(databaseName,user,password, hostaddr, port){
        
    };
    void createCitiesTable();
    void insertOperation();
   // void selectOperation();
};





