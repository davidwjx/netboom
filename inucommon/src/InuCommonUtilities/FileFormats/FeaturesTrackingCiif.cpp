#include "FeaturesTrackingCiif.h"

#include "HexHelper.h"
#include "Logger.h"
#include <iomanip>
#include <sstream>

#include <iostream>
#include <fstream>
#include <string>

constexpr int CIIF_FILE_VERSION{ 9 };

using namespace InuCommon;
using namespace std;

bool CFeaturesTrackingCiif::write(
    std::ostream& iOut,
    ECaptureMode iMode,
    FeaturesTracking::ParsedData& iData,
    unsigned int iCnt,
    unsigned int iSS,
    unsigned int iUID/* = std::numeric_limits<uint32_t>::max()*/,
    unsigned int iConfidence/* = std::numeric_limits<uint32_t>::max()*/)
{
    const uint32_t descriptorSize = sizeof(iData.Descriptor);

    switch (iMode)
    {
    case eDecimal:

        iOut << std::fixed << std::setprecision(7);

        iOut
            << "("
            << iData.X
            << ","
            << iData.Y
            << ","
            << iCnt
            << ") "
            << iData.Angle
            << " "
            << iData.IsRightImage
            << " "
            << iData.PatternSize
            << " "
            << iSS
            << " "
            ;

        if (iConfidence != std::numeric_limits<uint32_t>::max())
        {
            iOut << iUID
                << " "
                << iConfidence
                << " "
                ;
        }

        CHexHelper::outHexString(iOut, iData.Descriptor, descriptorSize);
        iOut << "\n";

        break;

    case eHexadecimal:

        iOut << std::fixed << std::setprecision(7);

        iOut
            << "# "
            << "("
            << iData.X
            << ","
            << iData.Y
            << ","
            << iCnt
            << ") "
            << iData.Angle
            << " "
            << iData.IsRightImage
            << " "
            << iData.PatternSize
            << " "
            << iSS
            << " ";

            if (iConfidence != std::numeric_limits<uint32_t>::max())
            {
                iOut << iUID
                    << " "
                    << iConfidence
                    << " ";
            }

            iOut << "\n";

        iOut << "(";
            CHexHelper::outHexString(iOut, iData.X);
            iOut << ",";
            CHexHelper::outHexString(iOut, iData.Y);
            iOut << ",";
            iOut << iCnt;
            iOut << ") ";
            CHexHelper::outHexString(iOut, iData.Angle);
            iOut << " ";
            iOut << iData.IsRightImage;
            iOut << " ";
            CHexHelper::outHexString(iOut, iData.PatternSize);
            iOut << " ";
            CHexHelper::outHexString(iOut, iSS);
            iOut << " ";

        if (iConfidence != std::numeric_limits<uint32_t>::max())
        {
            CHexHelper::outHexString(iOut, iUID);
            iOut << " ";
            CHexHelper::outHexString(iOut, iConfidence);
            iOut << " ";
        }

        CHexHelper::outHexString(iOut, iData.Descriptor, descriptorSize);
        iOut << "\n";

        break;

    case eBinary:
        //int rrr = sizeof(float);
        //rrr = sizeof(unsigned int);

        iOut.write(reinterpret_cast<char*>(&iData.X), sizeof(iData.X)); // float
        iOut.write(reinterpret_cast<char*>(&iData.Y), sizeof(iData.Y)); // float
        iOut.write(reinterpret_cast<char*>(&iCnt), sizeof(iCnt)); // unsigned int
        iOut.write(reinterpret_cast<char*>(&iData.Angle), sizeof(iData.Angle)); // float
        iOut.write(reinterpret_cast<char*>(&iData.IsRightImage), sizeof(iData.IsRightImage)); // unsigned int
        iOut.write(reinterpret_cast<char*>(&iData.PatternSize), sizeof(iData.PatternSize)); // unsigned int
        iOut.write(reinterpret_cast<char*>(&iSS), sizeof(iSS)); // unsigned int

        //InuCommon::CLogger::Write("Point number: " + to_string(iCnt) + " X: " + to_string(iData.X) + " Y: " + to_string(iData.Y) + " Angle: " + to_string(iData.Angle) + " IsRightImage: " + to_string(iData.IsRightImage), InuCommon::CLogger::eError, "Common");

        if (iConfidence != std::numeric_limits<uint32_t>::max())
        {
            iOut.write(reinterpret_cast<char*>(&iUID), sizeof(iUID));
            iOut.write(reinterpret_cast<char*>(&iConfidence), sizeof(iConfidence));
        }

        iOut.write((char*)iData.Descriptor, descriptorSize);

        break;

    default:
        return false;
    }
    return true;
}

bool CFeaturesTrackingCiif::write(
    FeaturesTracking::EOutputType iOutputType,
    ECaptureMode iCaptureMode,
    unsigned int iKeyPointNumber,
    unsigned int iKeyPointNumberRight,
    unsigned int iKeyPointNumberLeft,
    unsigned short iImageWidth,
    unsigned short iImageHeight,
    unsigned long long iFrameIndexForRecording,
    FeaturesTracking::EFeatureType iFeatureType,
    void* iData,
    std::string& iFileName,
    ERecordImage iImage)
{
    if (!iData)
    {
        return false;
    }

    std::ofstream fOut(iFileName, ios::out | ios::trunc | ios::binary);
    if (!fOut.good())
    {
        return false;
    }

    // TODO: Add commented header.
    fOut
        << "FormatVersion " << FeaturesTracking::FORMAT_VERSION<<"\n"
        << "OutputType " << int(iOutputType) << "\n"
        << "Encoding " << int(iCaptureMode) << "\n"
        << "KeyPoints " << iKeyPointNumber << " " << iKeyPointNumberLeft << " " << iKeyPointNumberRight << "\n"
        << "DescriptorSize " << FeaturesTracking::FT_DESCRIPTOR_SIZE << "\n"
        << "ImageWidth " << iImageWidth << "\n"
        << "ImageHeight " << iImageHeight << "\n"
        << "FrameIndexForRecording " << iFrameIndexForRecording << "\n"
        << "FeatureType " << iFeatureType << "\n"
        << "####" << "\n"
        ;

    uint32_t kp = 0;
    switch (iOutputType)
    {
    case FeaturesTracking::eParsed:
    {
        FeaturesTracking::ParsedData* pDataStart = (FeaturesTracking::ParsedData*)iData;
        const FeaturesTracking::ParsedData* pDataEnd = pDataStart + iKeyPointNumber;
        if (iImage == eBoth)
        {
            // This branch is for efficiency, to eleminate if statement in the for loop
            for (FeaturesTracking::ParsedData* pData = pDataStart; pData < pDataEnd; pData++)
            {
                write(fOut, iCaptureMode, *pData, kp++, 0);
            }
        }
        else if (iImage == eRight)
        {
            for (FeaturesTracking::ParsedData* pData = pDataStart; pData < pDataEnd; pData++)
            {
                if (pData->IsRightImage)
                {
                    write(fOut, iCaptureMode, *pData, kp++, 0);
                }
            }
        }
        else if (iImage == eLeft)
        {
            for (FeaturesTracking::ParsedData* pData = pDataStart; pData < pDataEnd; pData++)
            {
                if (!pData->IsRightImage)
                {
                    write(fOut, iCaptureMode, *pData, kp++, 0);
                }
            }
        }
    }
    break;

    case FeaturesTracking::eProcessed:
    {
        FeaturesTracking::ProcessedData* pDataStart = (FeaturesTracking::ProcessedData*)iData;
        const FeaturesTracking::ProcessedData* pDataEnd = pDataStart + iKeyPointNumber;
        if (iImage == eBoth)
        {
            // This branch is for efficiency, to eleminate if statement in the for loop
            for (FeaturesTracking::ProcessedData* pData = pDataStart; pData < pDataEnd; pData++)
            {
                write(fOut, iCaptureMode, *pData, kp++, 0, pData->UniqId, pData->Confidence);
            }
        }
        else if (iImage == eRight)
        {
            for (FeaturesTracking::ProcessedData* pData = pDataStart; pData < pDataEnd; pData++)
            {
                if (pData->IsRightImage)
                {
                    write(fOut, iCaptureMode, *pData, kp++, 0, pData->UniqId, pData->Confidence);
                }
            }
        }
        else if (iImage == eLeft)
        {
            for (FeaturesTracking::ProcessedData* pData = pDataStart; pData < pDataEnd; pData++)
            {
                if (!pData->IsRightImage)
                {
                    write(fOut, iCaptureMode, *pData, kp++, 0, pData->UniqId, pData->Confidence);
                }
            }
        }
        break;
    }
    case FeaturesTracking::eRaw:
    default:
        fOut << "Invalid output type " << iOutputType << endl;
        return false;

    }
    return true;
}


bool CFeaturesTrackingCiif::ReconstructFrameFromRecordedData(string& iFileName,
    FeaturesTracking::EOutputType& iOutputType,
    ECaptureMode& iCaptureMode,
    unsigned int& iKeyPointNumber,
    unsigned int& iKeyPointNumberRight,
    unsigned int& iKeyPointNumberLeft,
    unsigned short& iImageWidth,
    unsigned short& iImageHeight,
    unsigned long long& iFrameIndexForRecording,
    FeaturesTracking::EFeatureType& oFeatureType,
    void* iData
)
{
    std::string line;
    int descriptorSize;

    std::ifstream fIn(iFileName, ios::in | ios::binary);

    
    if (!fIn.good())
    {
        InuCommon::CLogger::Write("CFeaturesTrackingCiif::ReconstructFrameFromRecordedData Could not open " + iFileName , InuCommon::CLogger::eError, "Common");
        fIn.close();
        return false;
    }

    InuCommon::CLogger::Write("CFeaturesTrackingCiif::ReconstructFrameFromRecordedData Opened " + iFileName, InuCommon::CLogger::eDebug, "Common");
    
    // Read header
    getline(fIn, line);
    std::stringstream fStr(line);

    if (InuCommon::CHexHelper::expect(fStr, "FormatVersion ", "FormatVersion unavaliable"))
    {
        InuCommon::CLogger::Write("CFeaturesTrackingCiif::ReconstructFrameFromRecordedData failed to find FormatVersion in header", InuCommon::CLogger::eError, "Common");
        fIn.close();
        return false;
    }

    int formatVersion;
    fStr >> formatVersion;

    if (formatVersion != CIIF_FILE_VERSION)
    {
        InuCommon::CLogger::Write("CIIF File format version mismatch. Expected " + to_string(CIIF_FILE_VERSION) + ", got " + to_string(formatVersion), InuCommon::CLogger::eError, "Common");
        fIn.close();
        return false;
    }
    
    std::getline(fIn, line);
    std::stringstream fStrOutputType(line);

    if (CHexHelper::expect(fStrOutputType, "OutputType ", "OutputType unknown"))
    {
        InuCommon::CLogger::Write("CFeaturesTrackingCiif::ReconstructFrameFromRecordedData failed to find OutputType in header", InuCommon::CLogger::eError, "Common");
        fIn.close();
        return false;
    }

    int outputType;
    fStrOutputType >> outputType;
    iOutputType = FeaturesTracking::EOutputType(outputType);

    if (iOutputType != FeaturesTracking::EOutputType::eParsed)
    {
        InuCommon::CLogger::Write("CFeaturesTrackingCiif::ReconstructFrameFromRecordedData CIIF file OutputType is " + to_string(iOutputType) + " is not supported\n", InuCommon::CLogger::eError, "Common");
        fIn.close();
        return false;
    }
    

    std::getline(fIn, line);
    std::stringstream fStrEncodingType(line);

    if (CHexHelper::expect(fStrEncodingType, "Encoding ", "Encoding unavaliable"))
    {
        InuCommon::CLogger::Write("CFeaturesTrackingCiif::ReconstructFrameFromRecordedData failed to find Encoding in header", InuCommon::CLogger::eError, "Common");
        fIn.close();
        return false;
    }


    int captureMode;
    fStrEncodingType >> captureMode;
    iCaptureMode = ECaptureMode(captureMode);

    if (iCaptureMode != ECaptureMode::eBinary)
    {
        InuCommon::CLogger::Write("CIIF file Encoding (" + to_string(iCaptureMode) + ") not supported\n", InuCommon::CLogger::eError, "Common");
        fIn.close();
        return false;
    }

    

    
    std::getline(fIn, line);
    std::stringstream fStrKeyPoints(line);

    if (CHexHelper::expect(fStrKeyPoints, "KeyPoints ", "KeyPoints unavaliable"))
    {
        InuCommon::CLogger::Write("CFeaturesTrackingCiif::ReconstructFrameFromRecordedData failed to find KeyPoints in header", InuCommon::CLogger::eError, "Common");
        fIn.close();
        return false;
    }

    fStrKeyPoints >> iKeyPointNumber;
    fStrKeyPoints >> iKeyPointNumberLeft;
    fStrKeyPoints >> iKeyPointNumberRight;

    if (iKeyPointNumber != iKeyPointNumberLeft + iKeyPointNumberRight)
    {
        InuCommon::CLogger::Write("CIIF file num keypoints mismatch (" + to_string(iKeyPointNumber) + " != " + to_string(iKeyPointNumberLeft) + " + " + to_string(iKeyPointNumberRight) + ")\n", InuCommon::CLogger::eError, "Common");
        fIn.close();
        return false ;
    }
        
    std::getline(fIn, line);
    std::stringstream fStrDescriptorSize(line);

    if (CHexHelper::expect(fStrDescriptorSize, "DescriptorSize ", "DescriptorSize unavaliable"))
    {
        InuCommon::CLogger::Write("CFeaturesTrackingCiif::ReconstructFrameFromRecordedData failed to find DescriptorSize in header", InuCommon::CLogger::eError, "Common");
        fIn.close();
        return false;
    }

    fStrDescriptorSize >> descriptorSize;

    //if (descriptorSize != base->GetDescriptorSize())
    //{
    //    LOG_ERROR("Descriptor size mismatch");
    //    return;
    //}
    
    std::getline(fIn, line);
    std::stringstream fStrImageWidth(line);

    if (CHexHelper::expect(fStrImageWidth, "ImageWidth ", "ImageWidth unavaliable"))
    {
        InuCommon::CLogger::Write("CFeaturesTrackingCiif::ReconstructFrameFromRecordedData failed to find DescriptorSize in header", InuCommon::CLogger::eError, "Common");
        fIn.close();
        return false;
    }

    fStrImageWidth >> iImageWidth;
    
    std::getline(fIn, line);
    std::stringstream fStrImageHeight(line);

    if (CHexHelper::expect(fStrImageHeight, "ImageHeight ", "ImageWidth unavaliable"))
    {
        InuCommon::CLogger::Write("CFeaturesTrackingCiif::ReconstructFrameFromRecordedData failed to find ImageHeight in header", InuCommon::CLogger::eError, "Common");
        fIn.close();
        return false;
    }

    fStrImageHeight >> iImageHeight;
    

    std::getline(fIn, line);
    std::stringstream fStrIndexForRecording(line);

    if (CHexHelper::expect(fStrIndexForRecording, "FrameIndexForRecording ", "FrameIndexForRecording unavaliable"))
    {
        InuCommon::CLogger::Write("CFeaturesTrackingCiif::ReconstructFrameFromRecordedData failed to find FrameIndexForRecording in header", InuCommon::CLogger::eError, "Common");
        fIn.close();
        return false;
    }

    fStrIndexForRecording >> iFrameIndexForRecording;

    std::getline(fIn, line);
    std::stringstream fStrFeatureType(line);

    if (CHexHelper::expect(fStrFeatureType, "FeatureType ", "FeatureType unavaliable"))
    {
        InuCommon::CLogger::Write("CFeaturesTrackingCiif::ReconstructFrameFromRecordedData failed to find FeatureType in header", InuCommon::CLogger::eError, "Common");
        fIn.close();
        return false;
    }

    int featureType;
    fStrFeatureType >> featureType;
    oFeatureType = FeaturesTracking::EFeatureType(featureType);

    char* pData = (char*)iData;
    int counter = 0;
    
    while (fIn)
    {
        switch (iCaptureMode)
        {
        case ECaptureMode::eBinary:
        {
            // The sub-pixel refined extremum location at the fine resolution.
            float xs;
            float ys;
            unsigned int kpNum;
            unsigned int IsRightImage;
            unsigned int patternSize;
            // Anti-clockwise angle assignment in radians -pi, pi.
            // This field is filled after descriptor computation.
            float angle;
            // This corresponds to the sub-pixel index of the laplacian image
            // that the extremum was found.
            uint32_t ss;
                            
            fIn.read((char*)&xs, sizeof(xs));
            fIn.read((char*)&ys, sizeof(ys));
            fIn.read((char*)&kpNum, sizeof(kpNum));
            fIn.read((char*)&angle, sizeof(angle));
            fIn.read((char*)&IsRightImage, sizeof(IsRightImage));
            fIn.read((char*)&patternSize, sizeof(patternSize));
            fIn.read((char*)&ss, sizeof(ss));
   
            fIn.read(pData, descriptorSize);
            pData += descriptorSize;
            uint32_t ui_xs = uint32_t(xs * 16384);
            memcpy(pData, &ui_xs, sizeof(ui_xs));
            pData += sizeof(xs);
            uint32_t ui_ys = uint32_t(ys * 16384);
            memcpy(pData, &ui_ys, sizeof(ui_ys));
            pData += sizeof(ys);
            memcpy(pData, &angle, sizeof(angle));
            pData += sizeof(angle);
            memcpy(pData, &patternSize, sizeof(patternSize));
            pData += sizeof(patternSize);
            if (IsRightImage)
            {
                ss = 1;
            }
            else
            {
                ss = 0;
            }
            memcpy(pData, &ss, sizeof(ss));
            pData += sizeof(ss);    

            //InuCommon::CLogger::Write("XPoint number: " + to_string(kpNum) + " X: " + to_string(xs) + " Y: " + to_string(ys) + " Angle: " + to_string(angle) + " IsRightImage: " + to_string(IsRightImage), InuCommon::CLogger::eDebug, "Common");

            counter++;
            break;
        }

        case ECaptureMode::eDecimal:
        case ECaptureMode::eHexadecimal:
        {
            InuCommon::CLogger::Write("CFeaturesTrackingCiif::ReconstructFrameFromRecordedData unsupported ECaptureMode eDecimal or eHexadecimal", InuCommon::CLogger::eError, "Common");
            fIn.close();
            return false;
        }

        default:
        {
            InuCommon::CLogger::Write("CFeaturesTrackingCiif::ReconstructFrameFromRecordedData unsupported ECaptureMode", InuCommon::CLogger::eError, "Common");
            fIn.close();
            return false;
        }
        }

    }

     fIn.close();
     return true;
        
}