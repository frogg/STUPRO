#include <Utils/Database/CitiesDatabase.hpp>
#include <Utils/Misc/KronosLogger.hpp>

#include <QSqlError>
#include <QSqlResult>

const QString KITTY_QUERY_STRING("select * from city "
                                 "where lower(name) like lower(?) "
                                 "order by char_length(name), name, countrycode");

CitiesDatabase::CitiesDatabase(const QString databaseName, const QString user,
                               const QString password, const QString hostaddr, int port)
	: PostgresDB(databaseName, user, password, hostaddr, port), isPrepared(false) {
}

QList<City> CitiesDatabase::getCity(QString name) {
	if (!this->isPrepared) {
		this->query.prepare(KITTY_QUERY_STRING);
		this->isPrepared = true;
	}

	this->query.bindValue(0, QString("%%%1%%").arg(name));

	if (!this->query.exec() || this->query.lastError().type() != QSqlError::NoError) {
		throw QueryExecutionFailedException(this->query.lastError());
	}

	KRONOS_LOG_DEBUG("%s", this->query.executedQuery().toStdString().c_str());

	QList<City> cities;
	while (this->query.next()) {
		City result = {
			this->query.value(1).toString(),
			this->query.value(2).toString(),
			this->query.value(3).toFloat(),
			this->query.value(4).toFloat()
		};
		cities.append(result);
	}

	return cities;
}
