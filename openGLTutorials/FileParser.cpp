#include "FileParser.h"

std::vector<std::vector<std::string>> FileParser::parseCSV(std::vector<std::string>& csvData)
{
    std::vector<std::vector<std::string>> output;
    output.clear();

    if (csvData.empty())
        return output;

    std::vector<std::string> lineOutput;
    std::string delimiter = CSV_FILE_DELIM;
    std::string token;

    for (std::string line : csvData)
    {
        size_t last = 0;
        size_t next = 0;

        lineOutput.clear();

        while ((next = line.find(delimiter, last)) != std::string::npos)
        {
            token = line.substr(last, next - last);
            last = next + 1;
            lineOutput.push_back(token);
        }
        token = line.substr(last, next - last - 1);
        lineOutput.push_back(token);

        output.push_back(lineOutput);
    }

    return output;
}

std::vector<std::string> FileParser::formatCSV(std::vector<std::vector<std::string>> fileData)
{
    std::vector<std::string> output;
    output.clear();

    if (fileData.empty())
        return output;

    std::string lineFormated;

    for (std::vector<std::vector<std::string>>::iterator line = fileData.begin(); line != fileData.end(); line++)
    {
        lineFormated.clear();
        for (std::vector<std::string>::iterator token = (*line).begin(); token != (*line).end(); token++)
        {
            lineFormated.append(*token);
            if (next(token) != (*line).end())
                lineFormated.append(CSV_FILE_DELIM);
            else if (next(line) != fileData.end())
                lineFormated.append("\n");
        }
        output.push_back(lineFormated);
    }

    return output;
}

