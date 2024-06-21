#include <iostream>
#include <regex>

#include "TiffFile.h"
#include "CFileInfo.h"

CFileInfo::CFileInfo()
    : mWidth{ 0 }
    , mHeight{ 0 }
    , mBpp{ 0 }
    , format{ InuCommon::CTiffFile::ETiffFormat::eEmpty }
    , mBuffer{ nullptr }
{

}

CFileInfo::~CFileInfo()
{
    // do not release "mBuffer". It is may be saved for later use.
}

CFileInfo* CFileInfo::Factory(fs::path _filename)
{
    if (!fs::exists(_filename))
    {
        std::cerr << "Error: Input file \"" << _filename.string() << "\" does not exist\n";
        return nullptr;
    }

    CFileInfo* fi = new CFileInfo();

    fi->filename = _filename;

    unsigned char* buffer{ nullptr };

    InuCommon::CTiffFile::EError ret = InuCommon::CTiffFile::Read(
        fi->filename.string().c_str(),
        buffer,
        fi->mWidth,
        fi->mHeight,
        fi->mBpp,
        fi->format,
        fi->metaData
    );

    if (ret != InuCommon::CTiffFile::EError::eOK)
    {
        std::cerr << "\nError " << ret << " reading \"" << _filename.string() << "\" file";
        return nullptr;
    }

    fi->mBuffer = std::shared_ptr<uint16_t>(reinterpret_cast<uint16_t*>(buffer));

    return fi;
}

bool CFileInfo::DetailsMatch(CFileInfo& otherFi)
{
    return true
        && otherFi.mWidth == mWidth
        && otherFi.mHeight == mHeight
        && otherFi.mBpp == mBpp
        ;
}

void CFileInfo::FreeBuffer()
{

}

long long CFileInfo::GetFilenameTimestamp(fs::path fn)
{
    std::string f1 = fn.filename().stem().string();

    std::string ts1;

    if (f1.rfind("_") != std::string::npos)
    {
        ts1 = f1.substr(f1.rfind("_") + 1);
    }

    char* end;

    long long ts1Value = std::strtoll(ts1.c_str(), &end, 10);

    return ts1Value;
}

struct
{
    bool operator()(fs::path& _1, fs::path& _2) const
    {
        long long ts1Value = CFileInfo::GetFilenameTimestamp(_1);
        long long ts2Value = CFileInfo::GetFilenameTimestamp(_2);

        return ts1Value < ts2Value;
    }
}
dirFilenameTimestampComparer;

void CFileInfo::FindMatchingFilesInDir(
    fs::path dir,
    std::vector<fs::path>& names
)
{
    std::regex regexp("\\.tif");

    for (const auto& entry : fs::directory_iterator(dir))
    {
        if (!fs::is_regular_file(entry.path()))
        {
            continue;
        }

        const std::string extension = entry.path().extension().string();

        if (!std::regex_match(extension, regexp))
        {
            continue;
        }

        names.push_back(entry.path());
    }

    std::sort(names.begin(), names.end(), dirFilenameTimestampComparer);
}
