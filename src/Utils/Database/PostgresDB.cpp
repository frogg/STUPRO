#include <Utils/Database/PostgresDB.hpp>

#include <iostream>

using namespace std;
using namespace pqxx;

PostgresDB::PostgresDB(const std::string& databaseName, const std::string& user,
                       const std::string& password, const std::string& hostaddr, const std::string& port) {

	std::string databaseCreationInformation;
	databaseCreationInformation.append("dbname=").append(databaseName);
	databaseCreationInformation.append(" user=").append(user);
	databaseCreationInformation.append(" password=").append(password);
	databaseCreationInformation.append(" hostaddr=").append(hostaddr);
	databaseCreationInformation.append(" port=").append(port);
	this->dbConnection = new connection(databaseCreationInformation);

}

int PostgresDB::openDatabase() {
	if (this->dbConnection->is_open()) {
		cout << "Opened database successfully: " << this->dbConnection->dbname() << endl;
	} else {
		cout << "Can't open database" << endl;
		return 1;
	}
	return 0;
}

void PostgresDB::closeDatabase() {
	this->dbConnection->disconnect();
}
