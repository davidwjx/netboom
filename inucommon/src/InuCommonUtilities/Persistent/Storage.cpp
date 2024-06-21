#include "Storage.h"
#include "XmlStorage.h"
#include "FileSystemStorage.h"

using namespace InuCommon;

//////////////////////////////////////////////////////////////////////
//            CLASS IStorage IMPLEMENTATION                         //
//////////////////////////////////////////////////////////////////////

std::shared_ptr<InuCommon::IStorage> InuCommon::IStorage::CreateStorage(EStorageType type, const std::string& storageDir, const std::string& storageName)
{
    // At this stage only XML storage is created. It might be extended in the future
    if (type == IStorage::eXML)
    {
        return std::shared_ptr<IStorage>(new CXmlStorage(storageDir, storageName));
    }
    return std::shared_ptr<IStorage>(new CFileSystemStorage(storageDir, storageName));
}
