#include <pqxx/pqxx>

using namespace std;
using namespace pqxx;

class TestDb {
public:
    TestDb();

    int openDatabase();
    void closeDatabase();

    void createTable();
    void insertOperation();
    void selectOperation();

private:
    connection* dbConnection;
};
