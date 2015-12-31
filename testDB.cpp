#include <iostream>
#include <pqxx/pqxx> 

using namespace std;
using namespace pqxx;

void createTable();
void insertOperation();
void selectOperation();
int openDatabase();
void closeDatabase();

connection C("dbname=testdb user=stuproTest password=quappi \
             hostaddr=127.0.0.1 port=5432");

int main(int argc, char* argv[])
{
   try{
       openDatabase();
       createTable();
       //insertOperation();
       selectOperation();
       closeDatabase();
   }catch (const std::exception &e){
      cerr << e.what() << std::endl;
      return 1;
   }
}

int openDatabase(){
    if (C.is_open()) {
        cout << "Opened database successfully: " << C.dbname() << endl;
    } else {
        cout << "Can't open database" << endl;
        return 1;
    }
    return 0;
}

void closeDatabase(){
    C.disconnect ();
}

void createTable(){
    const char *sql = "CREATE TABLE IF NOT EXISTS CITIES("  \
    "ID INT PRIMARY KEY     NOT NULL," \
    "NAME           TEXT    NOT NULL," \
    "AGE            INT     NOT NULL," \
    "ADDRESS        CHAR(50)," \
    "SALARY         REAL );";
    work W(C);
    W.exec( sql );
    W.commit();
    cout << "Table created successfully or exits already" << endl;
}

void insertOperation(){
    const char *sql = "INSERT INTO CITIES (ID,NAME,AGE,ADDRESS,SALARY) "  \
    "VALUES (1, 'Paul', 32, 'California', 20000.00 ); " \
    "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
    "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "     \
    "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
    "VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );" \
    "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
    "VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";
    
    /* Create a transactional object. */
    work W(C);
    
    /* Execute SQL query */
    W.exec( sql );
    W.commit();
    cout << "Records created successfully" << endl;
}

void selectOperation(){
    /* Create SQL statement */
    const char* sql = "SELECT * from CITIES";
    
    /* Create a non-transactional object. */
    nontransaction N(C);
    
    /* Execute SQL query */
    result R( N.exec( sql ));
    
    /* List down all the records */
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        cout << "ID = " << c[0].as<int>() << endl;
        cout << "Name = " << c[1].as<string>() << endl;
        cout << "Age = " << c[2].as<int>() << endl;
        cout << "Address = " << c[3].as<string>() << endl;
        cout << "Salary = " << c[4].as<float>() << endl;
    }
    cout << "Operation done successfully" << endl;
}
