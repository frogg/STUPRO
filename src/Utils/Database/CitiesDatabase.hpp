#include <pqxx/pqxx>
#include <Utils/Database/PostgresDB.hpp>

using namespace std;
using namespace pqxx;

class CitiesDatabase : public PostgresDB {
public:
    void createTable();
    void insertOperation();
    void selectOperation();
};





