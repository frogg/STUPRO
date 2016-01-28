#include <CityListModel.hpp>

CityListModel::CityListModel(QObject* parent) {
	this->cities = cities;
}

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
		           QString::fromUtf8(city.name.c_str())
		           + " [" + QString::fromStdString(city.countryCode) + "] "
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

void CityListModel::add(City city) {
	this->beginInsertRows(QModelIndex(), this->cities.size(), this->cities.size());
	this->cities.push_back(city);
	this->endInsertRows();
}
