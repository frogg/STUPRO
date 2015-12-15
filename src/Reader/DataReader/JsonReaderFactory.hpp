#ifndef KRONOS_JSON_READER_FACTORY_HPP
#define KRONOS_JSON_READER_FACTORY_HPP

#include <Reader/DataReader/AbstractJsonReader.hpp>

class JsonReaderFactory {
public:
    /**
     * Create a JSON reader from a JSON file.
     * @param filename The JSON file's path
     * @return A JSON reader for the given file that will handle the file's contents with respect
     * to its meta information
     */
    static AbstractJsonReader createReader(QString filename);
};

#endif
