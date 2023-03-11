#include "FileParser.h"

std::vector<std::vector<std::string>> FileParser::parseCSV(std::vector<std::string>& csvData)
{
    std::vector<std::vector<std::string>> output;
    output.clear();

    if (csvData.empty())
        return output;

    std::vector<std::string> lineOutput;
    std::string delimiter = CSV_FILE_DELIM;

    for (std::string line : csvData)
    {
        lineOutput.clear();
        _parseOnDelim(line, lineOutput, delimiter);
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

void FileParser::parseOBJ(std::vector<std::string> objData, std::vector<std::vector<double>>& vector, std::vector<std::vector<double>>& texture, std::vector<std::vector<double>>& normal)
{
    if (!objData.size())
        return;

    std::vector<std::string> parsedLine;
    std::vector<std::string> parsedIndex;
    std::vector<std::vector<int>> indexing;
    std::vector<std::vector<double>> temp_geom;
    std::vector<std::vector<double>> temp_text;
    std::vector<std::vector<double>> temp_norm;
    std::string header;

    std::vector<double> geom_data;
    std::vector<double> text_data;
    std::vector<double> norm_data;

    std::vector<int> index_data;

    for (std::string line : objData)
    {
        header.clear();
        parsedLine.clear();

        if (line.find(OBJ_COMMENT_DELIM) != std::string::npos)
        {
            _parseOnDelim(line, parsedLine, OBJ_COMMENT_DELIM);
            line = parsedLine.at(0);
            parsedLine.clear();
        }

        if (!line.size())
            continue;

        _parseOnDelim(line, parsedLine, OBJ_DATA_DELIM);
        header = parsedLine.at(0);

        if (header == OBJ_NORM_DELIM)
        {
            norm_data.clear();
            for (int i = 0; i < 3; i++)
                norm_data.push_back(std::stod(parsedLine.at(i + 1)));
            temp_norm.push_back(norm_data);
        }

        else if (header == OBJ_TEXT_DELIM)
        {
            text_data.clear();
            for (int i = 0; i < 2; i++)
                text_data.push_back(std::stod(parsedLine.at(i + 1)));
            temp_text.push_back(text_data);
        }

        else if (header == OBJ_GEOM_DELIM)
        {
            geom_data.clear();
            for (int i = 0; i < 3; i++)
                geom_data.push_back(std::stod(parsedLine.at(i + 1)));
            temp_geom.push_back(geom_data);
        }

        else if (header == OBJ_FACE_DELIM)
        {
            index_data.clear();

            for (int i = 0; i < 3; i++)
            {
                parsedIndex.clear();
                _parseOnDelim(parsedLine.at(i + 1), parsedIndex, OBJ_INDEX_DELIM);

                for (std::string data : parsedIndex)
                    index_data.push_back(std::stoi(data) - 1);
            }

            indexing.push_back(index_data);
        }
    }

    for (int i = 0; i < indexing.size(); i++)
    {
        vector.push_back(temp_geom.at(indexing.at(i).at(0)));
        vector.push_back(temp_geom.at(indexing.at(i).at(3)));
        vector.push_back(temp_geom.at(indexing.at(i).at(6)));

        texture.push_back(temp_text.at(indexing.at(i).at(1)));
        texture.push_back(temp_text.at(indexing.at(i).at(4)));
        texture.push_back(temp_text.at(indexing.at(i).at(7)));

        normal.push_back(temp_norm.at(indexing.at(i).at(2)));
        normal.push_back(temp_norm.at(indexing.at(i).at(5)));
        normal.push_back(temp_norm.at(indexing.at(i).at(8)));
    }
}

void FileParser::_parseOnDelim(const std::string& line, std::vector<std::string>& parsed, const std::string& delim)
{
    size_t last = 0;
    size_t next = 0;
    std::string token;

    while ((next = line.find(delim, last)) != std::string::npos)
    {
        token = line.substr(last, next - last);
        last = next + 1;
        parsed.push_back(token);
    }

    token = line.substr(last, next - last - 1);
    parsed.push_back(token);
}

