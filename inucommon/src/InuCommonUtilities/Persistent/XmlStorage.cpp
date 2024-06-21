#include "XmlStorage.h"

#include <sstream>
#include <boost/filesystem.hpp>
#include "OSUtilities.h"

using namespace InuCommon;
using namespace std;


//////////////////////////////////////////////////////////////////////
//                      STATIC DEFINITIONS                          //
//////////////////////////////////////////////////////////////////////
const string  CXmlStorage::XML_EXTENSION(".xml");

//////////////////////////////////////////////////////////////////////
//          CLASS  CXmlStorage IMPLEMENTATION                       //
//////////////////////////////////////////////////////////////////////

bool CXmlStorage::Init()
{
    bool fileLoaded = false;

    boost::filesystem::path file(COsUtilities::ConvertEncoding(mStorageName));
    if (boost::filesystem::extension(file) != XML_EXTENSION)
    {
        // Remove XML extension (if exists)
        file += XML_EXTENSION;
    }
    else 
    {
        // remove extension, because other methods add it 
        mStorageName.resize(mStorageName.size() - XML_EXTENSION.size());
    }

    // At first look in current directory (for debugging)
    if (boost::filesystem::exists(file))
    {
        if (mParser.Load(COsUtilities::ConvertEncoding(file.string()).c_str()))
        {
            fileLoaded = true;
        }
    }

    // Otherwise look at input directory 
    if (!fileLoaded)
    {
        //Get file from %APPDATA% 
        if (mStorageDir.size() > 1) 
        {
            boost::filesystem::path fileFullPath(COsUtilities::ConvertEncoding(mStorageDir));
            fileFullPath /= file;
            if (mParser.Load(COsUtilities::ConvertEncoding(fileFullPath.string()).c_str()))
            {
                fileLoaded = true;
            }
        }
    }

    return fileLoaded;
}


bool CXmlStorage::Commit()
{
    boost::filesystem::path file(COsUtilities::ConvertEncoding(mStorageDir));
    file /= mStorageName.c_str();
    file += XML_EXTENSION.c_str();
    return mParser.Save(COsUtilities::ConvertEncoding(file.string()).c_str());
}


bool CXmlStorage::SelectTable(const std::string& tableName, bool doTraverse)
{
    if (!doTraverse)
    {
        mParser.ResetPos();

        // Open Root
        if (!mParser.FindElem(COsUtilities::ConvertEncoding(mStorageName)))
        {
            return false;
        }

        if (!mParser.IntoElem())
        {
            return false;
        }
    }

    // Open Tag
    if (!mParser.FindElem(COsUtilities::ConvertEncoding(tableName)))
    {
        return false;
    }

    return mParser.IntoElem();
}

bool CXmlStorage::AddTable(const std::string& tableName) 
{
    mParser.ResetPos();
    // Open Root
    bool ret = mParser.FindElem(COsUtilities::ConvertEncoding(mStorageName));
    if (!ret)
    {
        // Add root
        ret = mParser.AddElem(COsUtilities::ConvertEncoding(mStorageName));
    }
    if (!mParser.IntoElem())
    {
        ret = false;
    }
    else 
    {
        ret = mParser.AddElem(COsUtilities::ConvertEncoding(tableName));
    }
    
    return ret;
}

bool  CXmlStorage::GetNextRecord(std::string& key, std::string& value) 
{
    throw("TBD");
}

bool CXmlStorage::GetString(std::string& result, const std::string& key)
{
    result.clear();
    CXmlStorage*  nonConstPt=const_cast<CXmlStorage*>(this);
    nonConstPt->mParser.ResetMainPos();
    if (nonConstPt->mParser.FindElem(COsUtilities::ConvertEncoding(key)))
    {
        // Look for iValueName
        result = COsUtilities::ConvertEncodingBack(nonConstPt->mParser.GetData());
        return true;
    }
    return false; 
}

bool CXmlStorage::SetString(const std::string& key, const std::string& value)
{
    mParser.ResetMainPos(); 
    if (mParser.FindElem(COsUtilities::ConvertEncoding(key)))
    {
        return mParser.SetData(COsUtilities::ConvertEncoding(value));
    }

    return mParser.AddElem(COsUtilities::ConvertEncoding(key), COsUtilities::ConvertEncoding(value));
}
