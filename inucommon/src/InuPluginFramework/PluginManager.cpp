#ifdef _MSC_VER
#include "stdafx.h"
#endif

// TODO [Plugins]: this is temporary
//#define APR_PATH_MAX 260

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <iostream>
#include <string>

#include "DynamicLibrary.h"
#include "ObjectAdapter.h"
#include "PluginManager.h"
#include "Logger.h"

#define LOGGER_PREFIX_PLUGINS "PluginFramework"

// The registration params may be received from an external plugin so it is
// crucial to validate it, because it was never subjected to our tests.
static bool isValid(const char* iObjectName, const INU_RegisterParams* iParams)
{
    if (!iObjectName || !(*iObjectName))
    {
        return false;
    }

    if (!iParams || !iParams->createFunc || !iParams->destroyFunc)
    {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------

int PluginManager::RegisterObject(const char* iObjectName, const INU_RegisterParams* iParams)
{
    // Check parameters
    if (!isValid(iObjectName, iParams))
        return -1;

    PluginManager& pm = PluginManager::GetInstance();

    // Verify that versions match
    INU_PluginAPI_Version v = pm.mPlatformServices.version;
    if (v.major != iParams->version.major)
        return -1;

    std::string key((const char*)iObjectName);
    
    // [NL] no need for wildcards
    // If it's a wild card registration just add it
    //if (key == std::string("*")) {
    //    pm.mWildCardVec.push_back(*params);
    //    return 0;
    //}

    // If item already exists in eactMatc fail (only one can handle)
    //if (pm.mExactMatchMap.find(key) != pm.mExactMatchMap.end())
    //    return -1;

    // structure is:
    //   [map <serviceId, objects>]
    //     [service1]
    //      --> [map <name,object>]
    //         --> [object1]
    //         --> [object2]
    //     [service2]
    //      --> [map <name,object>]
    //         --> [object1]

    // TODO: [PLUGIN] Writer lock
    // WriteLock writeLock(mLocker);
    

    // Find-or-create, create a new service map if not exists
    auto& serviceMap = pm.mRegistratedObjectsMap[iParams->serviceTypeID];

    // Already exists
    if (serviceMap.find(key) != serviceMap.end()) {
        return -1;
    }

    //pm.mExactMatchMap[key] = *params;
    serviceMap[key] = *iParams;

    return 0;
}

// ---------------------------------------------------------------

PluginManager& PluginManager::GetInstance()
{
    static PluginManager instance;

    return instance;
}

int PluginManager::LoadAll(const std::string& iPluginDirectory, INU_InvokeServiceFunc iFunc)
{
    std::vector<boost::filesystem::path> ret;

    if (iPluginDirectory.empty()) // Check that the path is non-empty.
    {
        InuCommon::CLogger::Write("Plugins directory is empty, nothing to load", InuCommon::CLogger::eInfo, LOGGER_PREFIX_PLUGINS);
        return -1;
    }

    boost::filesystem::path root(iPluginDirectory);

    if (!boost::filesystem::exists(root) || !boost::filesystem::is_directory(root)) {
        InuCommon::CLogger::Write("Plugins directory missing " + root.string(), InuCommon::CLogger::eWarning, LOGGER_PREFIX_PLUGINS);
        return -1;
    }

    mPlatformServices.invokeService = iFunc;

    boost::filesystem::recursive_directory_iterator it(root);
    boost::filesystem::recursive_directory_iterator endit;

    std::string ext = CDynamicLibrary::GetExtension();

    while (it != endit) {
        if (boost::filesystem::is_regular_file(*it) && it->path().extension() == ext) {
            //std::cout << "Loading library:" << it->path().string() << "\n";

            ret.push_back(it->path().string());

            InuCommon::CLogger::Write("Loading plugin " + it->path().string(), InuCommon::CLogger::eInfo, LOGGER_PREFIX_PLUGINS);

            auto result = LoadByPath(it->path().string());
            InuCommon::CLogger::Write("Plugin load result " + std::to_string(result), InuCommon::CLogger::eInfo, LOGGER_PREFIX_PLUGINS);
            //std::cout << "result = " << result;
            //if (result == -1) {
            //    std::cout << " GetLastError = " << GetLastError();
            //}
            //std::cout << "\n";
        }

        ++it;
    }

    return 0;
}

int PluginManager::InitializePlugin(INU_InitFunc iInitFunc)
{

    try {
        PluginManager& pm = PluginManager::GetInstance();
        INU_ExitFunc exitFunc = iInitFunc(&pm.mPlatformServices);
        if (!exitFunc) {
            InuCommon::CLogger::Write("Plugin returned null exit function ", InuCommon::CLogger::eError, LOGGER_PREFIX_PLUGINS);
            return -1;
        }

        // Store the exit func so it can be called when unloading this plugin
        pm.mExitFuncVec.push_back(exitFunc);
        return 0;
    }
    catch (const std::exception& e)
    {
        InuCommon::CLogger::Write(std::string("Plugin exception, failed to init ") + e.what(), InuCommon::CLogger::eError, LOGGER_PREFIX_PLUGINS);
        return -1;
    }
}

PluginManager::PluginManager()
{
    mPlatformServices.version.major = 1;
    mPlatformServices.version.minor = 0;
    mPlatformServices.invokeService = NULL; // can be populated during loadAll()
    mPlatformServices.registerObject = RegisterObject;
    mPlatformServices.invokeId = 0;

    InuCommon::CLogger::SetSeverity("PluginFramework", InuCommon::CLogger::eInfo);
    InuCommon::CLogger::SetLogType(7);
}

PluginManager::~PluginManager()
{
    // Just in case it wasn't called earlier
    Shutdown();
}

int PluginManager::Shutdown()
{
    // TODO: [PLUGIN] Writer lock
    // WriteLock writeLock(mLocker);
    //std::lock_guard<std::shared_timed_mutex> writerLock(mRWMutex);

    int result = 0;
    for (ExitFuncVec::iterator func = mExitFuncVec.begin(); func != mExitFuncVec.end(); ++func) {
        try {
            InuCommon::CLogger::Write("Shutting down Plugin", InuCommon::CLogger::eInfo, LOGGER_PREFIX_PLUGINS);
            result = (*func)();
        } catch (...) {
            InuCommon::CLogger::Write("Exception while shutting down Plugin", InuCommon::CLogger::eInfo, LOGGER_PREFIX_PLUGINS);
            result = -1;
        }
    }

    mDynamicLibraryMap.clear();
    //mExactMatchMap.clear();
    mWildCardVec.clear();
    mRegistratedObjectsMap.clear();
    mExitFuncVec.clear();

    return result;
}

int PluginManager::LoadByPath(const std::string& iPluginPath)
{
    PluginManager& pm = PluginManager::GetInstance();

    //Path path(pluginPath);

    boost::filesystem::path path { boost::filesystem::canonical(boost::filesystem::path(iPluginPath)) };

    // TODO: check if canonical Resolve symbolic links
    /*#ifndef WIN32 
    if (path.isSymbolicLink())
    {
      char buff[APR_PATH_MAX+1];
      int length = ::readlink(path, buff, APR_PATH_MAX);
      if (length < 0)
        return -1;
        
      path = std::string(buff, length);
    }
    #endif*/

    // Don't load the same dynamic library twice
    if (mDynamicLibraryMap.find(path.string()) != mDynamicLibraryMap.end()) {
        InuCommon::CLogger::Write("Cannot load plugin twice " + path.string(), InuCommon::CLogger::eError, LOGGER_PREFIX_PLUGINS);
        return -1;
    }

    std::string errorString;
    CDynamicLibrary* d = LoadLibrary(path.string(), errorString);
    if (!d) {// not a dynamic library?
        InuCommon::CLogger::Write("Failed to load library " + path.string(), InuCommon::CLogger::eError, LOGGER_PREFIX_PLUGINS);
        return -1;
    }

    // Get the NTA_initPlugin() function
    INU_InitFunc initFunc = (INU_InitFunc)(d->GetSymbol("INU_initPlugin"));

    if (!initFunc) {// dynamic library missing entry point?
        InuCommon::CLogger::Write("Cannot find init function for " + path.string(), InuCommon::CLogger::eError, LOGGER_PREFIX_PLUGINS);
        return -1;
    }

    // assign invokeId
    unsigned long invokeId = 0;
    if (!mInvokeIds.empty()) {
        invokeId = mInvokeIds.rbegin()->first + 1;
    }

    pm.mInvokeIds[invokeId] = path.string();
    pm.mPlatformServices.invokeId = invokeId;

    int res = InitializePlugin(initFunc);
    if (res < 0) {// failed to initialize?
        InuCommon::CLogger::Write("Plugin failed to init " + std::to_string(res), InuCommon::CLogger::eError, LOGGER_PREFIX_PLUGINS);
        return res;
    }

    InuCommon::CLogger::Write("Done loading plugin " + path.string(), InuCommon::CLogger::eError, LOGGER_PREFIX_PLUGINS);
    return 0;
}


void* PluginManager::CreateObject(const std::string& iObjectType, unsigned int iServiceId, IObjectAdapter& iAdapter)
{
    std::cout << "Creating object " << iObjectType << "\n";
    // "*" is not a valid object type
    if (iObjectType == std::string("*"))
        return NULL;

    // Prepare object params
    INU_ObjectParams np;
    np.objectType = iObjectType.c_str();
    np.platformServices = &mPlatformServices;

    // TODO: [PLUGIN] Reader lock
    // ReadLock writeLock(mLocker);

    // Exact match found
    //auto it = mExactMatchMap.find(iObjectType);
    auto servicesIterator = mRegistratedObjectsMap.find(iServiceId);
    if (servicesIterator != mRegistratedObjectsMap.end()) {
        auto& objectsMap = servicesIterator->second;

        auto objectsIterator = objectsMap.find(iObjectType);
        if (objectsIterator != std::end(objectsMap)) {
            INU_RegisterParams& rp = objectsIterator->second;
            void* object = rp.createFunc(&np);
            if (object) {
                // Adapt if necessary (wrap C objects using an adapter)
                if (rp.programmingLanguage == INU_ProgrammingLanguage_C)
                    object = iAdapter.adapt(object, rp.destroyFunc);

                return object;
            }
        }
    }

    return NULL;
}

CDynamicLibrary* PluginManager::LoadLibrary(const std::string& iPath, std::string& oErrorString)
{
    CDynamicLibrary* d = CDynamicLibrary::Load(iPath, oErrorString);
    if (!d) // not a dynamic library?
        return NULL;

    // Add library to map, so it can be unloaded
    mDynamicLibraryMap[boost::filesystem::canonical(iPath).string()] = std::shared_ptr<CDynamicLibrary>(d);
    return d;
}

const PluginManager::ServicesRegistrationMap& PluginManager::GetServicesRegistrationMap()
{
    // TODO: [PLUGIN] this can be an issue, multi-threading
    return mRegistratedObjectsMap;
}

const PluginManager::DynamicLibraryMap& PluginManager::GetDynamicLibraryMap()
{
    return mDynamicLibraryMap;
}

INU_PlatformServices& PluginManager::GetPlatformServices()
{
    return mPlatformServices;
}
