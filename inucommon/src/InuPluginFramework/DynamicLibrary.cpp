#ifdef _MSC_VER
#include "stdafx.h"
#endif
#include "OSUtilities.h"
#include "DynamicLibrary.h"
#include <iostream>
#include <sstream>

// TODO: #include "OSUtilities.h"  // requires additional .lib dependency and include directory

CDynamicLibrary::CDynamicLibrary(void* iHandle)
    : mHandle(iHandle)
{
}

CDynamicLibrary::~CDynamicLibrary()
{
    if (mHandle) {
        InuCommon::COsUtilities::FreeTheLibrary(mHandle);
    }
}

std::string CDynamicLibrary::GetExtension()
{
#ifdef WIN32
    return ".dll";
#else
    return ".so";
#endif
}

CDynamicLibrary* CDynamicLibrary::Load(const std::string& iName,
    std::string& errorString)
{
    if (iName.empty()) {
        errorString = "Empty path.";
        return nullptr;
    }

    void* handle = InuCommon::COsUtilities::LoadTheLibrary(iName);

    if (handle == nullptr) {
        errorString = InuCommon::COsUtilities::GetLastErrorText();
        return nullptr;
    }

    return new CDynamicLibrary(handle);
}

void* CDynamicLibrary::GetSymbol(const std::string& iSymbol)
{
    if (!mHandle) {
        return nullptr;
    }

    return InuCommon::COsUtilities::GetFunctionEntryPoint(mHandle, iSymbol);
}
