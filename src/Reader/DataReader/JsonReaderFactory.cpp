#include <Utils/Config/JsonReaderFactory.hpp>

AbstractJsonReader JsonReaderFactory::createReader(const QString filename) const {
    // Open the JSON file while checking for potential errors
    QFile jsonFile(filename);
    QFileInfo jsonFileInfo(filename);

    if (!jsonFile.open(QIODevice::ReadOnly)) {
        throw JsonFileOpenException(jsonFileInfo.absoluteFilePath(), jsonFile.errorString());
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
        throw JsonParseException(
            jsonFileInfo.absoluteFilePath(),
            rapidjson::GetParseError_En(jsonDocument.GetParseError())
        );
    }
}
