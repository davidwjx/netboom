#include "OSUtilities.h"
#include "PngFile.h"
#include "lodepng.h"
#include "Logger.h"

using namespace InuCommon;

CPngFile::EError CPngFile::Write(const std::string& iFileName,
    const unsigned char* iBuffer,
    unsigned int iWidth,
    unsigned int iHeight,
    unsigned int iBytePerPixel,
    int iIndex)
{
    EError result = eOK;

    std::vector<unsigned char> image;
    image.assign(iBuffer, iBuffer + iWidth * iHeight * iBytePerPixel);

    //Encode the image
    unsigned int error = lodepng::encode(iFileName, iBuffer, iWidth, iHeight);

    //if there's an error, display it
    if (error)
    {
        result = eWriteData;
    }

    return result;
}

CPngFile::EError CPngFile::Read(const std::string& iFileName,
    unsigned char*& oBuffer,
    unsigned int& oWidth,
    unsigned int& oHeight)
{
    std::vector<unsigned char> image;

    //decode
    unsigned int error = lodepng::decode(image, oWidth, oHeight, iFileName);

    //if there's an error, display it
    if (error)
    {
        return eFileOpen;
    }

    size_t frameSize = image.size();

    oBuffer = new unsigned char[frameSize];

    memcpy(oBuffer, image.data(), frameSize);

    return eOK;
}
