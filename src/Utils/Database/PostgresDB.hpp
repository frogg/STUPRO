#include <pqxx/pqxx>

using namespace std;
using namespace pqxx;

/**
 * Postgres database
 */
class PostgresDB {
public:
    explicit PostgresDB(const std::string &databaseName, const std::string &user, const std::string &password, const std::string &hostaddr, const std::string &port);
    int openDatabase();
    void closeDatabase();

protected:
    connection* dbConnection;
};
