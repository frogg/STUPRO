#ifndef KRONOS_IMAGELAYERDESCRIPTION_HPP
#define KRONOS_IMAGELAYERDESCRIPTION_HPP

#include <QString>

struct ImageLayerDescription {
    QString name;
    QString baseUrl;
    QString format;
    int tileSize;
};

#endif
