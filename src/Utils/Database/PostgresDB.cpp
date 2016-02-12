#include <Utils/Database/PostgresDB.hpp>

PostgresDB::PostgresDB(const QString databaseName, const QString user,
                       const QString password, const QString hostaddr, int port)
	: database(QSqlDatabase::addDatabase("QPSQL")) {
	this->database.setHostName(hostaddr);
	this->database.setPort(port);
	this->database.setDatabaseName(databaseName);
	this->database.setUserName(user);
	this->database.setPassword(password);
}

void PostgresDB::openDatabase() {
	if (!this->database.open()) {
		throw DatabaseConnectionFailedException(this->database.lastError());
	}
}

void PostgresDB::closeDatabase() {
	this->database.close();
}
