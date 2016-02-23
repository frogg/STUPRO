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

	if (dataType == 6) {
		// If the expected data type is Double, the actual data type can be Integer or Double since
		// Integers are automatically converted to Doubles
		if (actualDataType == 5 || actualDataType == 6) {
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

		if (JsonValidator::getType(childDocument["timestamp"]) != 5) {
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

		if (JsonValidator::getType(childDocument["longitude"]) != 6 ||
		        JsonValidator::getType(childDocument["latitude"]) != 6) {
			throw JsonReaderParseException(
			    path,
			    QString("Error in a data element: The tags \"longitude\" and \"latitude\" should "
			            "be of type Double.")
			);
		}
	}

	JsonValidator::checkChildTag(childDocument, "children", 3, path);

	switch (dataType) {
	case Data::CITIES: {
		JsonValidator::checkChildTag(childDocument, "name", 4, path);
		break;
	}
	case Data::FLIGHTS: {
		JsonValidator::checkChildTag(childDocument, "startPosition", 2, path);
		JsonValidator::checkChildTag(childDocument, "endPosition", 2, path);
		JsonValidator::checkChildTag(childDocument["startPosition"], "latitude", 6, path);
		JsonValidator::checkChildTag(childDocument["startPosition"], "longitude", 6, path);
		JsonValidator::checkChildTag(childDocument["endPosition"], "latitude", 6, path);
		JsonValidator::checkChildTag(childDocument["endPosition"], "longitude", 6, path);
		JsonValidator::checkChildTag(childDocument["startPosition"], "airportCode", 4, path);
		JsonValidator::checkChildTag(childDocument["endPosition"], "airportCode", 4, path);
		JsonValidator::checkChildTag(childDocument, "airline", 4, path);
		break;
	}
	case Data::TWEETS: {
		JsonValidator::checkChildTag(childDocument, "author", 4, path);
		JsonValidator::checkChildTag(childDocument, "content", 4, path);
		break;
	}
	case Data::PRECIPITATION: {
		JsonValidator::checkChildTag(childDocument, "precipitationType", 4, path);
		JsonValidator::checkChildTag(childDocument, "precipitationRate", 6, path);
		break;
	}
	case Data::TEMPERATURE: {
		JsonValidator::checkChildTag(childDocument, "temperature", 6, path);
		break;
	}
	case Data::WIND: {
		JsonValidator::checkChildTag(childDocument, "direction", 6, path);
		JsonValidator::checkChildTag(childDocument, "speed", 6, path);
		break;
	}
	case Data::CLOUD_COVERAGE: {
		JsonValidator::checkChildTag(childDocument, "cloudCover", 6, path);
		break;
	}
	}
}