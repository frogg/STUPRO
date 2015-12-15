#ifndef KRONOS_JSON_READER_FACTORY_HPP
#define KRONOS_JSON_READER_FACTORY_HPP

#include <Reader/DataReader/AbstractJsonReader.hpp>
#include <QString>
#include <Utils/Misc/Macros.hpp>

#include <exception>

/**
 * Super class for all exceptions thrown while a JSON file is being loaded.
 */
struct ReaderException : public std::exception {
	std::string reason;

	ReaderException(QString reason) : reason(reason.toStdString()) { }

	const char *what() const KRONOS_NOTHROW override {
		return reason.c_str();
	}
};

/**
 * Exception thrown if the JSON file could not be opened.
 */
struct JsonFileOpenException : public ReaderException {
	JsonFileOpenException(QString path, QString errorDescription)
			: ReaderException(
				QString("The JSON file at '%1' could not be opened: %2")
				.arg(path).arg(errorDescription)
			) { }
};

/**
 * Exception thrown if the content of the JSON file could not be parsed.
 */
struct JsonParseException : public ReaderException {
	JsonParseException(QString path, QString errorDescription)
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
    static AbstractJsonReader createReader(const QString filename);
};

#endif
