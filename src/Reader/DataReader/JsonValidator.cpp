#include <Reader/DataReader/JsonValidator.hpp>

static QMap<int, QString> typeNameMap() {
    QMap<int, QString> map;
    map.insert(0, "Null");
    map.insert(1, "Boolean");
    map.insert(2, "Object");
    map.insert(3, "Array");
    map.insert(4, "String");
    map.insert(5, "Integer");
    map.insert(6, "Double");
    map.insert(7, "Unknown");
    return map;
}
const QMap<int, QString> JsonValidator::TYPE_NAMES = typeNameMap();

static QMap<QString, int> nonTemporalTagsMap() {
    QMap<QString, int> map;
    map.insert("temporal", 1);
    map.insert("dataType", 4);
    return map;
}
const QMap<QString, int> JsonValidator::NON_TEMPORAL_TAGS = nonTemporalTagsMap();

int JsonValidator::getType(rapidjson::Value& jsonValue) {
    if (jsonValue.IsNull()) {
        return 0;
    } else if (jsonValue.IsBool()) {
        return 1;
    } else if (jsonValue.IsObject()) {
        return 2;
    } else if (jsonValue.IsArray()) {
        return 3;
    } else if (jsonValue.IsString()) {
        return 4;
    } else if (jsonValue.IsInt() || jsonValue.IsInt64() ||
            jsonValue.IsUint() || jsonValue.IsUint64()) {
        return 5;
    } else if (jsonValue.IsDouble()) {
        return 6;
    } else {
        return 7;
    }
}

void JsonValidator::validateMetaData(rapidjson::Value& metaDocument, QString path) {
    QMap<QString, int>::iterator i;
    QMap<QString, int> map = JsonValidator::NON_TEMPORAL_TAGS;
    
    for (i = map.begin(); i != map.end(); ++i) {
        if (!metaDocument.HasMember(i.key().toStdString().c_str())) {
            throw JsonReaderParseException(
                path,
                QString("Error in the meta header: The tag \"%1\" does not exist.").arg(i.key())
            );
        }
        
        if (JsonValidator::getType(metaDocument[i.key().toStdString().c_str()]) != i.value()) {
            throw JsonReaderParseException(
                path,
                QString("Error in the meta header: The tag \"%1\" should be of type %2 but is "
                        "of type %3.").arg(i.key(), JsonValidator::TYPE_NAMES.value(i.value()),
                        JsonValidator::TYPE_NAMES.value(
                            JsonValidator::getType(metaDocument[i.key().toStdString().c_str()])
                        ))
            );
        }
    }
    
    // This check can only be done this late since we only know by now that the temporal tag exists
    if (metaDocument["temporal"].GetBool()) {
        if (!metaDocument.HasMember("timeResolution")) {
            throw JsonReaderParseException(
                path,
                QString("Error in the meta header: The tag \"timeResolution\" does not exist "
                        "but is mandatory for temporal data.")
            );
        }
        
        if (JsonValidator::getType(metaDocument["timeResolution"]) != 5) {
            throw JsonReaderParseException(
                path,
                QString("Error in the meta header: The tag \"timeResolution\" should be of type "
                        "Integer but is of type %1.")
                        .arg(JsonValidator::TYPE_NAMES.value(
                            JsonValidator::getType(metaDocument[i.key().toStdString().c_str()])
                        ))
            );
        }
    }
}