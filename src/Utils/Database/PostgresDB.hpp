#ifndef KRONOS_UTILS_DATABASE_POSTGRESDB_HPP
#define KRONOS_UTILS_DATABASE_POSTGRESDB_HPP

#include <Utils/Misc/Exceptions.hpp>
#include <QSqlDatabase>
#include <QSqlError>
#include <QString>

struct DatabaseConnectionFailedException : public KronosException {
	DatabaseConnectionFailedException(QSqlError error) : KronosException(error.text()) { }
};

struct QueryExecutionFailedException : public KronosException {
	QueryExecutionFailedException(QSqlError error) : KronosException(error.text()) { }
};

/**
 * Postgres database
 */
class PostgresDB {
public:
	explicit PostgresDB(const QString databaseName, const QString user,
	                    const QString password, const QString hostaddr, int port);
	void openDatabase();
	void closeDatabase();

protected:
	QSqlDatabase database;
};

#endif
