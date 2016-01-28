#ifndef KRONOS_JSON_VALIDATOR_HPP
#define KRONOS_JSON_VALIDATOR_HPP

#include <QString>
#include <QMap>

#include <Reader/DataReader/Data.hpp>
#include <Reader/DataReader/JsonReaderFactory.hpp>
#include <Utils/Misc/Macros.hpp>

#include <rapidjson/rapidjson.h>

class JsonValidator {
public:
    /**
     * Check whether the given JSON document contains valid and full meta data information, throw
     * a `JsonReaderParseException` if it does not.
     * @param metaDocument The JSON document to be checked
     * @param temporal Boolean value denoting whether the meta data should contain time information
     * @param path The file path so it can be included in the exception description
     */
    static void validateMetaData(rapidjson::Value& metaDocument, QString path);
    
    /**
     * Check whether the given JSON document contains valid and full information sufficient for
     * describing a child element in the tree of data points, throw a `JsonReaderParseException` if
     * it does not.
     * @param childDocument The JSON document to be checked
     * @param dataType The data type the JSON document should be checked for
     * @param path The file path so it can be included in the exception description
     */
    static void validateChildElement(rapidjson::Value& childDocument, Data::Type dataType,
            QString path);
private:
    /**
     * Get the type of a JSON value in integer notation.
     * @param jsonValue The value to be checked
     * @return The type of the value in integer notation
     */
    static int getType(rapidjson::Value& jsonValue);
    
	/**
	 * This QMap maps the internal integer notation of JSON attribute types to human-readable
     * strings.
	 */
	static const QMap<int, QString> TYPE_NAMES;
    
    /**
     * A map of all tags along with their data types that should be present in a non-temporal
     * meta header.
     */
    static const QMap<QString, int> NON_TEMPORAL_TAGS;
};

#endif
