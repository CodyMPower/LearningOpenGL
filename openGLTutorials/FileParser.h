#pragma once
#include <vector>
#include <string>

#define CSV_FILE_DELIM ","

class FileParser
{
public:
	std::vector<std::vector<std::string>> parseCSV(std::vector<std::string>& csvData);
	std::vector<std::string> formatCSV(std::vector<std::vector<std::string>> fileData);
};

