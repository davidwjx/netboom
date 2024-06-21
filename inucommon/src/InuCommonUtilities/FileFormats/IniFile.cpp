#include "IniFile.h"

#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>

using namespace InuCommon;
using namespace std;

using boost::property_tree::ptree;

bool CIniFile::Load(const std::string& iFileName)
{    
    boost::system::error_code ec;
    bool exists(boost::filesystem::exists(COsUtilities::ConvertEncoding(iFileName), ec));
    if (exists == false || ec.value() != boost::system::errc::success)
    {
        return false;
    }

    try
    {
        read_ini(iFileName, mCurrentFileContents);
    }
    catch (...)
    {
    }
    
    return true;
}


bool CIniFile::Save(const std::string& iFileName)
{
    try
    {
        write_ini(iFileName, mCurrentFileContents);
    }
    catch (...)
    {
        return false;
    }

    return true;
}


bool CIniFile::GetValue(const std::string& iKey, std::string& oValue)
{    
    oValue.clear();

    try
    {
        // change to boost::optional, so it want throw an excption in case field do not exist
        if (boost::optional<string> oi = mCurrentFileContents.get_optional<string>(iKey))
        {
            oValue = *oi;
        }

    }
    catch (const std::exception&)
    {
        return false;
    }

    return true; 
}

bool CIniFile::SetValue(const std::string& iKey, const std::string& iValue)
{  
    // Set the value
    mCurrentFileContents.put<std::string>(iKey, iValue);

    return true;
}

bool CIniFile::SetValue(const std::string& iSection, const std::string& iKey, const std::string& iValue)
{
    char delimiter = '.';

    // Validity check: if delimiter can be found in iSection or iKey, the parsing would be ambiguous.
    if (iSection.find(delimiter) != std::string::npos || iKey.find(delimiter) != std::string::npos)
    {
        return false;
    }

    return SetValue(iSection + delimiter + iKey, iValue);
}

bool CIniFile::GetValue(const std::string& iSection, const std::string& iKey, std::string& oValue)
{
    char delimiter = '.';

    // Validity check: if delimiter can be found in iSection or iKey, the parsing would be ambiguous.
    if (iSection.find(delimiter) != std::string::npos || iKey.find(delimiter) != std::string::npos)
    {
        return false;
    }

    return GetValue(iSection + delimiter + iKey, oValue);
}


