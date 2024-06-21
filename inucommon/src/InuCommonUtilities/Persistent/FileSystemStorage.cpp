#include "OSUtilities.h"
#include "FileSystemStorage.h"

#include <sstream>
#include <boost/filesystem.hpp>

using namespace InuCommon;
using namespace std;

bool CFileSystemStorage::Init()
{
    boost::filesystem::path file(mStorageDir);
    file /= mStorageName.c_str();

    if (boost::filesystem::is_directory(file))
    {
        // On one computer opening of ifstream with directory succeeded and read from it failed with excpetion
        return true;
    }

    std::ifstream tempFileHandler;
    tempFileHandler.open(InuCommon::COsUtilities::ConvertEncoding(file.string()));    
    if (tempFileHandler.good())
    {
        mCurrentFileContents.clear();
        mCurrentFileContents << tempFileHandler.rdbuf();
        return !(mCurrentFileContents.bad());
    }

    return false;
}


bool CFileSystemStorage::Commit()
{
    boost::filesystem::path file(mStorageDir);
    if (mTableName.empty())
    {
        file /= mStorageName.c_str();
    }
    else
    {
        file /= mTableName.c_str();

    }

    std::ofstream tempFileHandler;
    tempFileHandler.open(InuCommon::COsUtilities::ConvertEncoding(file.string()));    
    if (tempFileHandler.good())
    {
        string tmp(mCurrentFileContents.str());
        tempFileHandler.write(tmp.c_str(), tmp.size());
        return true;
    }
    return false;
}


bool CFileSystemStorage::SelectTable(const std::string& tableName, bool doTraverse)
{
    mTableName = tableName;

    boost::filesystem::path file(mStorageDir);
    if (file.string().empty())
    {
        file = mTableName.c_str();
    }
    else
    {
        file /= mTableName.c_str();
    }
    file += ".txt";  // Add extension 

    boost::system::error_code ec;
    bool exists(boost::filesystem::exists(COsUtilities::ConvertEncoding(file.string()),ec));
    if (exists == false || ec.value() != boost::system::errc::success)
    {
        // Look for csv extension
        file = boost::filesystem::path(mStorageDir);
        file /= mTableName.c_str();
        file += ".csv";  // Add extension 
    }

    std::ifstream tempFileHandler;
    tempFileHandler.open(InuCommon::COsUtilities::ConvertEncoding(file.string()));    
    if (tempFileHandler.good())
    {
        mCurrentFileContents.clear();
        mCurrentFileContents << tempFileHandler.rdbuf();
        tempFileHandler.close();
        return !(mCurrentFileContents.bad());
    }

    return false;
}

bool CFileSystemStorage::GetString(std::string& result, const std::string& key)
{
    bool ret = false;

    result.clear();

    string currentKey, value;

    // Find next token
    ret = GetNextRecord(currentKey, value, mCurrentFileContents);
    if (ret == true)
    {
        if (currentKey != key)
        {
            mCurrentFileContents.seekg(0, ios_base::beg);
            do 
            {
                ret = GetNextRecord(currentKey, value, mCurrentFileContents);
            } while (ret && currentKey != key && !mCurrentFileContents.eof() && mCurrentFileContents.good());

            if (ret == false || currentKey != key )
            {
                // We must return to the beginning of the file (text string) in order to be able to read the rest of tokens
                mCurrentFileContents.clear();
                mCurrentFileContents.seekg(0, ios_base::beg);
                ret = mCurrentFileContents.good();
            }
        }

        if (ret && currentKey == key)
        {
            result = value;
        }
    }
    return ret; 
}

bool CFileSystemStorage::SetString(const std::string& key, const std::string& value)
{
    // Currently is just appends the key and doesn't replace it, if it already exists
    mCurrentFileContents << key << "=" << value << endl;

    return mCurrentFileContents.good();
}


bool CFileSystemStorage::GetNextRecord(std::string& key, std::string& record, istream& inputFile)
{
    static const int LINE_LENGTH=4096;
    static char buffer[LINE_LENGTH];
    memset(buffer, 0, LINE_LENGTH);

    bool ret(false);
    do 
    {
        // Read next line
        inputFile.getline(buffer, LINE_LENGTH);
        string   line(buffer);

        // Skip lines which begins with '#' (comments)
        if (!line.empty() && line[0] != '#')
        {
            // Linux support - reomve /r
            if (line[line.size() -1] == '\r')
            {
                line.resize(line.size() -1);
            }
            // Break the line into key and record which are separated by ' ' or '='
            size_t pos = line.find('=',0);
            if (pos == string::npos || (pos == line.size() - 1))
            {
                pos = line.find(',',0);
                if (pos == string::npos || (pos == line.size() - 1))
                {
                    pos = line.find(' ',0);
                    if (pos == string::npos || (pos == line.size() - 1))
                    {
                        break;
                    }
                }
            }
            key = line.substr(0 ,pos);
            pos++;
            record = line.substr(pos, line.size() - pos);
            ret = true;
        }
    } while (!ret && !inputFile.eof() && inputFile.good());

    return ret;
}

bool   CFileSystemStorage::GetNextRecord(std::string& key, std::string& value) 
{
    return GetNextRecord(key, value, mCurrentFileContents); 
}
