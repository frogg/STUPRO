#ifndef KRONOS_JSON_READER_FACTORY_HPP
#define KRONOS_JSON_READER_FACTORY_HPP

#include <QString>
#include <QMap>

#include <Reader/DataReader/JsonReader.hpp>
#include <Reader/DataReader/Data.hpp>
#include <Utils/Misc/Macros.hpp>
#include <Utils/Misc/Exceptions.hpp>

#include <exception>
#include <memory>

/**
 * Super class for all exceptions thrown while a JSON file is being loaded.
 */
struct ReaderException : public KronosException {
	ReaderException(QString reason) : KronosException(reason) { }
};

/**
 * Exception thrown if the JSON file could not be opened.
 */
struct JsonReaderFileOpenException : public ReaderException {
	JsonReaderFileOpenException(QString path, QString errorDescription)
		: ReaderException(
		      QString("The JSON file at '%1' could not be opened: %2")
		      .arg(path).arg(errorDescription)
		  ) { }
};

/**
 * Exception thrown if the content of the JSON file could not be parsed.
 */
struct JsonReaderParseException : public ReaderException {
	JsonReaderParseException(QString path, QString errorDescription)
		: ReaderException(
		      QString("The JSON file at '%1' could not be parsed: %2")
		      .arg(path).arg(errorDescription)
		  ) { }
};

class JsonReaderFactory {
public:
	/**
	 * Create a JSON reader from a JSON file.
	 * @param filename The JSON file's path
	 * @return A JSON reader for the given file that will handle the file's contents with respect
	 * to its meta information
	 */
	static std::unique_ptr<JsonReader> createReader(const QString filename);
private:
	/**
	 * This QMap maps the string notation of a data type (e.g. 'cities') as present in a JSON file's
	 * meta header to the respective type's enum value (e.g. `Data::CITIES`).
	 */
	static const QMap<QString, Data::Type> DATA_TYPES;
};

#endif
