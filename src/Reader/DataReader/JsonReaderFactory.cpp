#include <Reader/DataReader/JsonReaderFactory.hpp>

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QPair>
#include <Utils/Misc/MakeUnique.hpp>
#include <Reader/DataReader/JsonValidator.hpp>
#include <rapidjson/error/en.h>

// Workaround to make static initialization possible in the IDE we all love -- Visual Studio.
static QMap<QString, Data::Type> dataTypeMap() {
	QMap<QString, Data::Type> map;

	map.insert("cities", Data::CITIES);
	map.insert("flights", Data::FLIGHTS);
	map.insert("tweets", Data::TWEETS);
	map.insert("precipitation", Data::PRECIPITATION);
	map.insert("temperature", Data::TEMPERATURE);
	map.insert("wind", Data::WIND);
	map.insert("cloudCover", Data::CLOUD_COVERAGE);

	return map;
}

const QMap<QString, Data::Type> JsonReaderFactory::DATA_TYPES = dataTypeMap();

std::unique_ptr<JsonReader> JsonReaderFactory::createReader(const QString filename) {
	// Open the JSON file while checking for potential errors
	QFile jsonFile(filename);
	QFileInfo jsonFileInfo(filename);

	if (!jsonFile.open(QIODevice::ReadOnly)) {
		throw JsonReaderFileOpenException(jsonFileInfo.absoluteFilePath(), jsonFile.errorString());
	}

	// Read from the opened stream and save the file's contents in a string
	QTextStream in(&jsonFile);
	QString configText = in.readAll();
	jsonFile.close();

	// Parse the file's content and construct a representation inside the memory
	// using a `rapidjson::Document`
	rapidjson::Document jsonDocument;
	jsonDocument.Parse(configText.toStdString().c_str());

    if (jsonDocument.HasParseError()) {
        throw JsonReaderParseException(
            jsonFileInfo.absoluteFilePath(),
            QString("The file's JSON content is invalid. At position %1: %2")
                .arg(
                    QString::number(jsonDocument.GetErrorOffset()),
                    QString(rapidjson::GetParseError_En(jsonDocument.GetParseError()))
                )
        );
    }
    
    // Check if there is a root
    if (!jsonDocument.HasMember("root")) {
        throw JsonReaderParseException(
            jsonFileInfo.absoluteFilePath(),
            "The file does not contain a root data tag."
        );
    }

    // Extract meta data and create a new JSON reader
    if (!jsonDocument.HasMember("meta")) {
        throw JsonReaderParseException(
            jsonFileInfo.absoluteFilePath(),
            "The file does not contain meta information."
        );
    }
    
    rapidjson::Value& metaData = jsonDocument["meta"];
    
    // Now that we know it exists, check the meta tag for validity
    JsonValidator::validateMetaData(metaData, jsonFileInfo.absoluteFilePath());
    
    bool temporal = metaData["temporal"].GetBool();
    
    std::unique_ptr<JsonReader> jsonReader;
    
    if (temporal) {
        jsonReader = makeUnique<JsonReader>(
            jsonDocument["root"],
            JsonReaderFactory::DATA_TYPES.value(QString(metaData["dataType"].GetString())),
			jsonFileInfo.absoluteFilePath(),
            true,
            metaData["timeResolution"].GetInt()
        );
    } else {
        jsonReader = makeUnique<JsonReader>(
            jsonDocument["root"],
            JsonReaderFactory::DATA_TYPES.value(QString(metaData["dataType"].GetString())),
			jsonFileInfo.absoluteFilePath(),
            false,
            0
        );
    }
    
    return std::move(jsonReader);
}
