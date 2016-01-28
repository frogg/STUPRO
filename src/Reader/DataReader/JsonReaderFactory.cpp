#include <Reader/DataReader/JsonReaderFactory.hpp>

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QPair>
#include <Utils/Misc/MakeUnique.hpp>
#include <rapidjson/error/en.h>

#include <Reader/DataReader/DataType.hpp>

// Workaround to make static initialization possible in the IDE we all love -- Visual Studio.
static QMap<QString, int> dataTypeMap() {
	QMap<QString, int> map;

	map.insert("cities", DataType::CITIES);
	map.insert("flights", DataType::FLIGHTS);
	map.insert("tweets", DataType::TWEETS);
	map.insert("precipitation", DataType::PRECIPITATION);
	map.insert("temperature", DataType::TEMPERATURE);
	map.insert("wind", DataType::WIND);
	map.insert("cloudCover", DataType::CLOUDCOVER);

	return map;
}

const QMap<QString, int> JsonReaderFactory::DATA_TYPES = dataTypeMap();

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
		    rapidjson::GetParseError_En(jsonDocument.GetParseError())
		);
	}

	// Extract meta data and create a new JSON reader
	rapidjson::Value& metaData = jsonDocument["meta"];
	std::unique_ptr<JsonReader> jsonReader = makeUnique<JsonReader>(
	            jsonDocument["root"],
	            JsonReaderFactory::DATA_TYPES.value(QString(metaData["dataType"].GetString())),
	            metaData["temporal"].GetBool()
	        );

	return std::move(jsonReader);
}
