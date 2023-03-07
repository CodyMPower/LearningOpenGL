#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define NO_ERROR "No Error"
#define ERROR_OPENING_FILE "Could Not Open File"
#define NO_FILE_SPECIFIED "No File Specified To Open"

class FileMan
{
public:
	FileMan();
	FileMan(FileMan& manager);

	void setFilePath(std::string filePath) { this->filePath = filePath; }
	void setFileName(std::string fileName) { this->fileName = fileName; }
	void setMode(std::fstream::openmode mode) { this->mode = mode; }
	operator bool() { return this->file.is_open(); }
	int openFile();
	int openFile(std::fstream::openmode mode);
	int openFile(std::string filePath, std::string fileName);
	int openFile(std::string filePath, std::string fileName, std::fstream::openmode mode);
	std::string getErrorMessage() { return this->errorMessage; }
	std::string getFilePath();
	std::vector<std::string> readFile();
	int writeFile(std::vector<std::string> data);
	int testWriteFile();

	~FileMan();

private:
	std::fstream file;
	std::string errorMessage;
	std::string filePath;
	std::string fileName;
	std::fstream::openmode mode;
};

