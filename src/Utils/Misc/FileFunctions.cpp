#include "FileFunctions.hpp"

#include <fstream>

std::string readFile(std::string filename)
{
	std::string content;
	std::ifstream file(filename);
	if (!file)
	{
		return content;
	}
	file.seekg(0, std::ios::end);
	content.resize(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(&content[0], content.size());
	return content;
}
