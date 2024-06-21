#include "OSUtilities.h"
#include "CsvFile.h"

using namespace InuCommon;
using namespace std;

#include <boost/tokenizer.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <fstream>
#include <iostream>
#include <string>

const std::string CCsvFile::REVISION_NAME("#Revision");

CCsvFile::EError CCsvFile::ReadWithoutSpaces(const string& iFileName, vector<vector<string>>& oBuffer, string& oRevision)
{
    // Parse the CSV file removing spaces from strings
    return Read(iFileName, oBuffer,true, oRevision);
}

CCsvFile::EError CCsvFile::Read(const string& iFileName, vector<vector<string>>& oBuffer, string& oRevision)
{
    // Parse the CSV file with default value, empty string - no delimiters to remove
    return Read(iFileName, oBuffer, false, oRevision);
}

CCsvFile::EError CCsvFile::Read(const string& iFileName, vector<vector<string>>& oBuffer,const bool iTrimLine, string& oRevision)
{
    oRevision.clear();
    oBuffer.clear();

    ifstream csvFile(iFileName);

    CCsvFile::EError result = eOK;

    if (csvFile.bad())
    {
        result = eFileOpen;
    }
    else
    {
        string line;

        while (getline(csvFile, line) && csvFile.good())
        {
            if (line.empty())
            {
                continue;
            }
            else if (line[0] == '#')
            {
                if (line.find(REVISION_NAME) != 0)
                {
                    continue;
                }
            }

            // Split the line into it cells and push it to the output buffer
            vector<string> tokens;
            boost::char_separator<char> sep(",", "" , boost::keep_empty_tokens);
            boost::tokenizer<boost::char_separator<char>> tokensBoost(line, sep);

            for (boost::tokenizer<boost::char_separator<char>>::iterator iter = tokensBoost.begin();
                iter != tokensBoost.end();
                ++iter)
            {
                std::string strToken = *iter;
                if (iTrimLine == true)
                {
                    // Remove whitespaces from end and start of string
                    boost::algorithm::trim(strToken);
                }
                tokens.push_back(strToken);
            }

            if ((tokens.size() > 1) && tokens[0] == REVISION_NAME)
            {
                oRevision = tokens[1];
            }
            else
            {
                oBuffer.push_back(tokens);
            }
        }
        csvFile.close();
    }

    return result;
}
