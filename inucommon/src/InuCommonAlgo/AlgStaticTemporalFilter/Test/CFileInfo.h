#pragma once

#if defined(_MSC_VER)
    #include <filesystem>
    namespace fs = std::filesystem;
#elif defined(__ANDROID__) || defined(__linux__)
    #include <boost/filesystem.hpp>
    namespace fs = boost::filesystem;
#endif

#include "OSUtilities.h"

class CFileInfo
{
public:

    fs::path filename;
    std::shared_ptr<uint16_t> mBuffer;
    unsigned int mWidth, mHeight, mBpp;
    InuCommon::CTiffFile::ETiffFormat format;
    InuCommon::CTiffFile::CMetaData metaData;

    CFileInfo();

    ~CFileInfo();

    static CFileInfo* Factory(fs::path _filename);

    bool DetailsMatch(CFileInfo& otherFi);

    void FreeBuffer();

    static void FindMatchingFilesInDir(
        fs::path dir,
        std::vector<fs::path>& names
    );

    static long long GetFilenameTimestamp(fs::path fn);
};


