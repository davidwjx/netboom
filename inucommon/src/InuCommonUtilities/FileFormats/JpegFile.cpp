#include "OSUtilities.h"
#include "JpegFile.h"
#include "jpgd.h"
#include "jpge.h"

using namespace InuCommon;

const int CJpegFile::mTypeOfMatrix = 3;

CJpegFile::EError CJpegFile::Read(const std::string& iFileName,
    unsigned char*& oBuffer,
    int& oSizeOfBuffer,
    int& oWidth,
    int& oHeight)
{
    int actualType;
    oBuffer = jpgd::decompress_jpeg_image_from_file(iFileName.c_str(), &oWidth, &oHeight, &actualType, mTypeOfMatrix);
    if (!oBuffer)
    {
        return eFileOpen;
    }

    if (actualType != 3)
    {
        return eUnsupportedType;
    }

    oSizeOfBuffer = oWidth * oHeight * mTypeOfMatrix ;
    return eOK;
}

CJpegFile::EError CJpegFile::Write(const std::string& iFileName,
    const unsigned char* iBuffer,
    int iwidth,
    int iHeight,
    int iQuality)
{
    jpge::params compressParams;
    compressParams.m_quality = iQuality;
    if (jpge::compress_image_to_jpeg_file(iFileName.c_str(), iwidth, iHeight, mTypeOfMatrix, iBuffer, compressParams))
    {
        return eOK;
    }
    return eWriteData;
}

