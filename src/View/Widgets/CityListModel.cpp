#include <CityListModel.hpp>

CityListModel::CityListModel(QObject* parent) { }

int CityListModel::rowCount(const QModelIndex& parent) const {
	return this->cities.size();
}

QVariant CityListModel::data(const QModelIndex& index, int role) const {
	if (!index.isValid() || index.row() >= this->cities.size()) {
		return QVariant();
	}

	if (role == Qt::DisplayRole) {
		City city = this->cities.at(index.row());
		return QVariant(
		           city.name
		           + " [" + city.countryCode + "] "
		           + QString::number(city.latitude) + QString::fromUtf8("\u00b0 N ")
		           + QString::number(city.longitude) + QString::fromUtf8("\u00b0 E")
		       );
	} else {
		return QVariant();
	}
}

void CityListModel::clear() {
	this->beginRemoveRows(QModelIndex(), 0, this->cities.size());
	this->cities.clear();
	this->endRemoveRows();
}

void CityListModel::beginAdd(int count) {
	this->beginInsertRows(QModelIndex(), this->cities.size(), this->cities.size() + count);
}

void CityListModel::endAdd() {
	this->endInsertRows();
}

void CityListModel::add(City city) {
	this->cities.append(city);
}

void CityListModel::add(QList<City> cities) {
	this->cities.append(cities);
}

const QList<City> CityListModel::getAll() const {
	return this->cities;
}
