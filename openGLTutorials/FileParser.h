#pragma once
#include <vector>
#include <string>

#define CSV_FILE_DELIM ","

#define OBJ_COMMENT_DELIM "#"
#define OBJ_DATA_DELIM " "
#define OBJ_GEOM_DELIM "v"
#define OBJ_NORM_DELIM "vn"
#define OBJ_TEXT_DELIM "vt"
#define OBJ_FACE_DELIM "f"
#define OBJ_INDEX_DELIM "/"

class FileParser
{
public:
	std::vector<std::vector<std::string>> parseCSV(std::vector<std::string>& csvData);
	std::vector<std::string> formatCSV(std::vector<std::vector<std::string>> fileData);
	void parseOBJ(std::vector<std::string> objData, std::vector<std::vector<double>>& vector, std::vector<std::vector<double>>& texture, std::vector<std::vector<double>>& normal);

private:
	void _parseOnDelim(const std::string& line, std::vector<std::string>& parsed, const std::string& delim);
};

