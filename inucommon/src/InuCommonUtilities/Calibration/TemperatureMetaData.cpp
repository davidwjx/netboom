#include "OSUtilities.h"
#include "TemperatureMetaData.h"
#include "boost/filesystem.hpp"
#include "boost/lexical_cast.hpp"
#include <boost/assign/list_of.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <iostream>
using namespace InuCommon;

const std::string CTemperatureMetaData::mMetaDataFileExtension = ".ini";

const std::map<CTemperatureMetaData::EDataFields, std::string> CTemperatureMetaData::mNamesMap = boost::assign::map_list_of<CTemperatureMetaData::EDataFields, std::string>
(eName, "Name")
(eActual, "Average Temperature (Actual)")
(eRequired, "Average Temperature (Required)");



CTemperatureMetaData::CTemperatureMetaData(const std::string& iName, int iAvrageTemperatureActual, int iAvrageTemperatureRequired)
{
    mValuesMap[eName] = iName;
    mValuesMap[eActual] = boost::lexical_cast<std::string>(iAvrageTemperatureActual);
    mValuesMap[eRequired] = boost::lexical_cast<std::string>(iAvrageTemperatureRequired);
}

CTemperatureMetaData::EMetaDataError CTemperatureMetaData::Read(const boost::filesystem::path& iDirPath)
{
    // Try to read file in this level. 
    for (boost::filesystem::directory_iterator fileEnd, fileIter(iDirPath); fileIter != fileEnd; ++fileIter)
    {
        boost::system::error_code boostErr;
        if ( is_regular_file(fileIter->path(), boostErr) &&
             (boostErr.value() == boost::system::errc::success) &&
             (fileIter->path().extension().string() == CTemperatureMetaData::mMetaDataFileExtension) )
        {
            boost::property_tree::ptree pt;
            try
            {
                boost::property_tree::ini_parser::read_ini(fileIter->path().string(), pt);

                for (int i = 0; i <= eRequired; ++i)
                {
                    EDataFields currentField = static_cast<EDataFields>(i);
                    mValuesMap[currentField] = pt.get<std::string>(mNamesMap.at(currentField));
                }

                mValuesMap[eFolderName] = iDirPath.string();

                return eOK;
            }
            catch (boost::property_tree::ptree_error& )
            {
                return eFileParseError;
            }

            return eUnexpectedException;
        }
    }

    return eFileNotExists;
}




CTemperatureMetaData::EMetaDataError CTemperatureMetaData::Write(const std::string& iFilePath) const
{
    boost::property_tree::ptree pt;
    try
    {
        for (int i = 0; i <= eRequired; ++i)
        {
            EDataFields currentField = static_cast<EDataFields>(i);
            pt.put(mNamesMap.at(currentField), mValuesMap.at(currentField));
        }

        boost::property_tree::ini_parser::write_ini(iFilePath, pt);
        return eOK;
    }
    catch (boost::property_tree::ptree_bad_data& )
    {
        return eInvalidData;
    }
    catch (boost::property_tree::ini_parser::ini_parser_error& )
    {
        return eFileParseError;
    }

    return eUnexpectedException;
}

CTemperatureMetaData::EMetaDataError CTemperatureMetaData::GetIntVal(EDataFields iField, int& oVal) const
{
    std::string valAsString;
    if (GetStringVal(iField, valAsString) != eOK)
    {
        return eDataCannotBeObtained;
    }

    if (!boost::conversion::try_lexical_convert(valAsString, oVal))
    {
        return eDataCannotBeObtained;
    }

    return eOK;
}

CTemperatureMetaData::EMetaDataError CTemperatureMetaData::GetStringVal(EDataFields iField, std::string& oVal) const
{
    try
    {
        oVal = mValuesMap.at(iField);
        return eOK;
    }
    catch (std::out_of_range& )
    {
        return eDataCannotBeObtained;
    }
}

CTemperatureMetaData::EMetaDataError CTemperatureMetaData::GetAllMetaFiles(const std::string& iPathToDirectory, std::map<int, CTemperatureMetaData>& oMetaData)
{
    oMetaData.clear();
    CTemperatureMetaData tempMetaData;

    try
    {
        for (boost::filesystem::directory_iterator dirEnd, dirIter(iPathToDirectory); dirIter != dirEnd; ++dirIter)
        {
            boost::system::error_code boostErr;
            if (is_directory(dirIter->path(), boostErr) &&
                (boostErr.value() == boost::system::errc::success))
            {
                if (tempMetaData.Read(dirIter->path()) == eOK)
                {
                    int actualTemp;
                    if (tempMetaData.GetActualTemperature(actualTemp) == eOK)
                    {
                        std::swap(oMetaData[actualTemp], tempMetaData);
                    }
                }
            }
        }

        return eOK;
    }
    catch (boost::filesystem::filesystem_error& )
    {
        return eFileSystemViolation;
    }
}
