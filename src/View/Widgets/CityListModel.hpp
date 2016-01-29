#ifndef KRONOS_VIEW_WIDGETS_CITY_LIST_MODEL_HPP
#define KRONOS_VIEW_WIDGETS_CITY_LIST_MODEL_HPP

#include <QAbstractListModel>
#include <Utils/City.hpp>

class CityListModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit CityListModel(QObject* parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    void clear();
    void beginAdd(int count);
    void add(City city);
    void endAdd();
    const std::vector<City> getAll() const;

private:
    std::vector<City> cities;
};

#endif
