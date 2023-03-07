#include "FileMan.h"

FileMan::FileMan()
{
	errorMessage = NO_ERROR;
}

FileMan::FileMan(FileMan& manager)
{
	this->fileName = manager.fileName;
	this->filePath = manager.filePath;
	this->errorMessage = manager.errorMessage;
	this->mode = std::fstream::in | std::fstream::out;
}

int FileMan::openFile()
{
	return openFile(this->filePath, this->fileName, this->mode);
}

int FileMan::openFile(std::fstream::openmode mode)
{
	return openFile(this->filePath, this->fileName, mode);
}

int FileMan::openFile(std::string filePath, std::string fileName)
{
	return openFile(filePath, fileName, this->mode);
}

int FileMan::openFile(std::string filePath, std::string fileName, std::fstream::openmode mode)
{
	if (file.is_open())
		file.close();

	if (this->fileName.empty() || this->filePath.empty()) {
		this->errorMessage = NO_FILE_SPECIFIED;
		return 0;
	}

	std::string fullPath = this->filePath;
	fullPath.append("\\" + this->fileName);

	try
	{
		file.open(fullPath, mode);

		if (!file)
		{
			file.open(fullPath, std::fstream::out);
			file.close();
			file.open(fullPath, mode);
		}
	}
	catch (std::ios_base::failure& e)
	{
		errorMessage = ERROR_OPENING_FILE;
		return 0;
	}

	return 1;
}

std::string FileMan::getFilePath()
{
	std::string output;
	output.clear();

	output.append(this->filePath);
	output.append("\\" + this->fileName);

	return output;
}

std::vector<std::string> FileMan::readFile()
{
	std::string line;
	std::vector<std::string> output;
	output.clear();

	if (!this->file.is_open())
		if (!openFile())
			return output;

	while (std::getline(file, line))
		output.push_back(line);

	file.close();

	return output;
}

int FileMan::writeFile(std::vector<std::string> data)
{
	if (!this->file.is_open())
		if (!openFile())
			return 0;

	if (data.empty())
	{
		this->errorMessage = "No Data To Write";
		return 0;
	}

	for (std::string line : data)
		this->file << line;

	file.close();

	return 1;
}

int FileMan::testWriteFile()
{
	if (!this->file.is_open())
		if (!openFile())
			return 0;

	for (int i = 0; i < 10; i ++)
		this->file << "Hello!";

	file.close();

	return 1;
}

FileMan::~FileMan()
{
	if (file.is_open())
		file.close();
	errorMessage.clear();
	filePath.clear();
	fileName.clear();
}
