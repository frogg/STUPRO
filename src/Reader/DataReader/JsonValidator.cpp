#include <Reader/DataReader/JsonValidator.hpp>

#define kJsonTypeNull 0
#define kJsonTypeBoolean 1
#define kJsonTypeObject 2
#define kJsonTypeArray 3
#define kJsonTypeString 4
#define kJsonTypeInteger 5
#define kJsonTypeDouble 6
#define kJsonTypeUnknown 7



static QMap<int, QString> typeNameMap() {
	QMap<int, QString> map;
	map.insert(kJsonTypeNull, "Null");
	map.insert(kJsonTypeBoolean, "Boolean");
	map.insert(kJsonTypeObject, "Object");
	map.insert(kJsonTypeArray, "Array");
	map.insert(kJsonTypeString, "String");
	map.insert(kJsonTypeInteger, "Integer");
	map.insert(kJsonTypeDouble, "Double");
	map.insert(kJsonTypeUnknown, "Unknown");
	return map;
}
const QMap<int, QString> JsonValidator::TYPE_NAMES = typeNameMap();

static QMap<QString, int> nonTemporalTagsMap() {
	QMap<QString, int> map;
	map.insert("temporal", kJsonTypeBoolean);
	map.insert("dataType", kJsonTypeString);
	return map;
}
const QMap<QString, int> JsonValidator::NON_TEMPORAL_TAGS = nonTemporalTagsMap();

int JsonValidator::getType(rapidjson::Value& jsonValue) {
	if (jsonValue.IsNull()) {
		return kJsonTypeNull;
	} else if (jsonValue.IsBool()) {
		return kJsonTypeBoolean;
	} else if (jsonValue.IsObject()) {
		return kJsonTypeObject;
	} else if (jsonValue.IsArray()) {
		return kJsonTypeArray;
	} else if (jsonValue.IsString()) {
		return kJsonTypeString;
	} else if (jsonValue.IsInt() || jsonValue.IsInt64() ||
	           jsonValue.IsUint() || jsonValue.IsUint64()) {
		return kJsonTypeInteger;
	} else if (jsonValue.IsDouble()) {
		return kJsonTypeDouble;
	} else {
		return kJsonTypeUnknown;
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

		if (JsonValidator::getType(metaDocument["timeResolution"]) != kJsonTypeInteger) {
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

void JsonValidator::checkChildTag(rapidjson::Value& jsonValue, QString memberName, int dataType,
                                  QString path) {
	if (!jsonValue.HasMember(memberName.toStdString().c_str())) {
		throw JsonReaderParseException(
		    path,
		    QString("Error in a data element: The data point is missing its \"%1\" tag.")
		    .arg(memberName)
		);
	}

	int actualDataType = JsonValidator::getType(jsonValue[memberName.toStdString().c_str()]);
	bool expectedEqualsActual = false;

	if (dataType == kJsonTypeDouble) {
		// If the expected data type is Double, the actual data type can be Integer or Double since
		// Integers are automatically converted to Doubles
		if (actualDataType == kJsonTypeInteger || actualDataType == kJsonTypeDouble) {
			expectedEqualsActual = true;
		}
	} else {
		if (actualDataType == dataType) {
			expectedEqualsActual = true;
		}
	}

	if (!expectedEqualsActual) {
		throw JsonReaderParseException(
		    path,
		    QString("Error in a data element: The tag \"%1\" should be of type %2 but is of type "
		            "%3.")
		    .arg(
		        memberName,
		        JsonValidator::TYPE_NAMES.value(dataType),
		        JsonValidator::TYPE_NAMES.value(
		            JsonValidator::getType(jsonValue[memberName.toStdString().c_str()])
		        )
		    )
		);
	}
}

void JsonValidator::validateChildElement(rapidjson::Value& childDocument, Data::Type dataType,
        QString path) {
	if (Data::isTemporal(dataType)) {
		if (!childDocument.HasMember("timestamp")) {
			throw JsonReaderParseException(
			    path,
			    QString("Error in a data element: The data point is temporal but does not contain "
			            "a \"timestamp\" tag.")
			);
		}

		if (JsonValidator::getType(childDocument["timestamp"]) != kJsonTypeInteger) {
			throw JsonReaderParseException(
			    path,
			    QString("Error in a data element: The tag \"timestamp\" should be of type "
			            "Integer but is of type %1.")
			    .arg(
			        JsonValidator::TYPE_NAMES.value(
			            JsonValidator::getType(childDocument["timestamp"])
			        )
			    )
			);
		}
	}

	if (dataType != Data::FLIGHTS) {
		if (!childDocument.HasMember("longitude") || !childDocument.HasMember("latitude")) {
			throw JsonReaderParseException(
			    path,
			    QString("Error in a data element: The data point is missing its \"longitude\" or "
			            "\"latitude\" tag.")
			);
		}

		if (JsonValidator::getType(childDocument["longitude"]) != kJsonTypeDouble ||
		        JsonValidator::getType(childDocument["latitude"]) != kJsonTypeDouble) {
			throw JsonReaderParseException(
			    path,
			    QString("Error in a data element: The tags \"longitude\" and \"latitude\" should "
			            "be of type Double.")
			);
		}
	}

	JsonValidator::checkChildTag(childDocument, "children", kJsonTypeArray, path);

	switch (dataType) {
	case Data::CITIES: {
		JsonValidator::checkChildTag(childDocument, "name", kJsonTypeString, path);
		break;
	}
	case Data::FLIGHTS: {
		JsonValidator::checkChildTag(childDocument, "startPosition", kJsonTypeObject, path);
		JsonValidator::checkChildTag(childDocument, "endPosition", kJsonTypeObject, path);
		JsonValidator::checkChildTag(childDocument["startPosition"], "latitude", kJsonTypeDouble, path);
		JsonValidator::checkChildTag(childDocument["startPosition"], "longitude", kJsonTypeDouble, path);
		JsonValidator::checkChildTag(childDocument["endPosition"], "latitude", kJsonTypeDouble, path);
		JsonValidator::checkChildTag(childDocument["endPosition"], "longitude", kJsonTypeDouble, path);
		JsonValidator::checkChildTag(childDocument["startPosition"], "airportCode", kJsonTypeString, path);
		JsonValidator::checkChildTag(childDocument["endPosition"], "airportCode", kJsonTypeString, path);
		JsonValidator::checkChildTag(childDocument, "airline", kJsonTypeString, path);
		break;
	}
	case Data::TWEETS: {
		JsonValidator::checkChildTag(childDocument, "author", kJsonTypeString, path);
		JsonValidator::checkChildTag(childDocument, "content", kJsonTypeString, path);
        JsonValidator::checkChildTag(childDocument, "numberOfRetweets", kJsonTypeInteger, path);
		break;
	}
	case Data::PRECIPITATION: {
		JsonValidator::checkChildTag(childDocument, "precipitationType", kJsonTypeString, path);
		JsonValidator::checkChildTag(childDocument, "precipitationRate", kJsonTypeDouble, path);
		break;
	}
	case Data::TEMPERATURE: {
		JsonValidator::checkChildTag(childDocument, "temperature", kJsonTypeDouble, path);
		break;
	}
	case Data::WIND: {
		JsonValidator::checkChildTag(childDocument, "direction", kJsonTypeDouble, path);
		JsonValidator::checkChildTag(childDocument, "speed", kJsonTypeDouble, path);
		break;
	}
	case Data::CLOUD_COVERAGE: {
		JsonValidator::checkChildTag(childDocument, "cloudCover", kJsonTypeDouble, path);
		break;
	}
	}
}