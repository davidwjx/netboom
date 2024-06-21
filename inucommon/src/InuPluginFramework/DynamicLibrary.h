#ifndef DYNAMIC_LIBRARY_H
#define DYNAMIC_LIBRARY_H

#include <string>

class CDynamicLibrary {
public:
    static CDynamicLibrary* Load(const std::string& iPath, std::string& oErrorString);
    static std::string GetExtension();
    void* GetSymbol(const std::string& iName);
    ~CDynamicLibrary();


private:
    CDynamicLibrary();
    CDynamicLibrary(void* iHandle);
    CDynamicLibrary(const CDynamicLibrary&);

private:
    void* mHandle;
};

#endif
