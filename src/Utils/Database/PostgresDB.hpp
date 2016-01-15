#include <pqxx/pqxx>

using namespace std;
using namespace pqxx;

class PostgresDB {
public:
    PostgresDB(const std::string &databaseName, const std::string &user, const std::string &password, const std::string &hostaddr, const std::string &port);
    int openDatabase();
    void closeDatabase();

protected:
    connection* dbConnection;
};
