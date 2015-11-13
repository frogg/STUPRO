#ifndef KRONOS_IMAGELAYERDESCRIPTION_HPP
#define KRONOS_IMAGELAYERDESCRIPTION_HPP

#include <qstring.h>

struct ImageLayerDescription {
    QString baseUrl;
    QString format;
    int tileSize;
};

#endif
