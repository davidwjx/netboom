///////////////////////////////////////////////////////////
//  CInuModelDB.cpp
//  Implementation of the Class CInuModelDB
//  Created on:      09-���-2015 15:41:25
//  Original author: oren
///////////////////////////////////////////////////////////


#include "InuModelDB.h"
#include "Locks.h"
#include "Storage.h"
#include "OSUtilities.h"

#include <algorithm>
#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "CsvFile.h"


using namespace InuCommon;
using namespace std;

std::shared_ptr<CInuModelDB>	CInuModelDB::sTheInstance;

void CInuModelDB::CreateTheInstance()
{
    // 'sConstructionLocker' must be a local member, otherwise there is an error when 
    // trying to load code as DLL (CommonUtilitiesCSharpWrapper.dll)
    static BLock sConstructionLocker;
    WriteLock wLock(sConstructionLocker);

    // Check again (after lock) that the object has not been created yet.
    if (sTheInstance == nullptr) 
    {
        sTheInstance = std::shared_ptr<CInuModelDB>(new CInuModelDB());        
    }
}

bool CInuModelDB::Load()
// This function is called only by singleton creation which protects from concurrent load. 
{
    std::string data;
    
    std::string fileName(CInuModel::GetInuModelsDBPath());

    vector<vector<string>> buffer;
    string revision;
    if (CCsvFile::Read(fileName, buffer, revision) != InuCommon::CCsvFile::eOK)
    {
        return false;
    }
    else if (revision != CInuModel::INUMODELDB_REVISION)
    {
        return false;
    }

    // insert to vector the first word in each column
    for (auto model : buffer)
    {
        // skip revision line
        if (boost::iequals(model[0], CInuModel::TITLES_NAME) || boost::iequals(model[0], "Model@Name"))
        {
            continue;
        }

        CInuModel sensorModel;
        
        if (sensorModel.ParseData(model) != eOK)
        {
            // Failed to load model information
            return false; 
        }

        mInuModels.insert(std::make_pair(make_pair(sensorModel.GetModelName(), sensorModel.GetHwTypeAsString()), sensorModel));
    }

    return true;
}
