
#include "OSUtilities.h"
#include "TiffFile.h"


#include "tiffio.h"

#include <boost/filesystem.hpp>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cfloat>

#if (defined(_MSC_VER) && (_MSC_VER <1700)) //Visual studio version before 2012
#include <boost/thread/mutex.hpp>
using namespace boost;
#pragma warning(disable:4482)
#else
#include <mutex>
#endif

using namespace InuCommon;
using namespace std;


const std::string CTiffFile::TIFF_FILE_EXT(".tif");
const std::string CTiffFile::INUITIVE_VENDOR_NAME("Inuitive");
std::map<CTiffFile::ETiffFormat,  std::string> CTiffFile::FORMAT_NAME;

bool CTiffFile::sIsInitialized(false);

#include "limits.h"
// It is not a member of class to prevent code pollution
static TIFFExtendProc _ParentExtender = nullptr;


void CTiffFile::TagExtender(TIFF* tif)
{

    TIFFFieldInfo tiffFieldInfo[] = { {TIFFTAG_INU_SENSORID, -1, -1, TIFF_ASCII, FIELD_CUSTOM, true, false, (char*)"InuSensorID"},
                                      {TIFFTAG_INU_SENSORNAME, -1, -1, TIFF_ASCII, FIELD_CUSTOM, true, false, (char*)"InuSensorName"},
                                      {TIFFTAG_INU_REVISION, 1, 1, TIFF_SHORT, FIELD_CUSTOM, true, false, (char*)"InuCalibrationRevision"},
                                      {TIFFTAG_INU_RESOLUTION, 1, 1, TIFF_SHORT, FIELD_CUSTOM, true, false, (char*)"InuResolution"},
                                      {TIFFTAG_INU_INPUTTYPE, 1, 1, TIFF_SHORT, FIELD_CUSTOM, true, false, (char*)"InuInterleaved"},
                                      {TIFFTAG_INU_FORMAT, 1, 1, TIFF_SHORT, FIELD_CUSTOM, true, false, (char*)"(char*)InuInputType"},
                                      {TIFFTAG_INU_ADDIONAL_DATA, -1, -1, TIFF_ASCII, FIELD_CUSTOM, true, false, (char*)"InuAddtionalData"},
                                      {TIFFTAG_INU_SENSOR_MODEL, -1, -1, TIFF_ASCII, FIELD_CUSTOM, true, false, (char*)"InuSensorModel"},
                                      {TIFFTAG_INU_HW_TYPE, 1, 1, TIFF_SHORT, FIELD_CUSTOM, true, false, (char*)"InuHWType"},
                                      {TIFFTAG_INU_DISPARITY_OFFSET, 1, 1, TIFF_SHORT, FIELD_CUSTOM, true, false, (char*)"DisparityOffset"},
                                      {TIFFTAG_INU_FRAME_INDEX, 1, 1, TIFF_LONG, FIELD_CUSTOM, true, false, (char*)"FrameIndex"},
                                      {TIFFTAG_INU_TIMESTAMP_HIGH, 1, 1, TIFF_LONG, FIELD_CUSTOM, true, false, (char*)"TimestampHigh"},
                                      {TIFFTAG_INU_TIMESTAMP_LOW, 1, 1, TIFF_LONG, FIELD_CUSTOM, true, false, (char*)"TimestampLow"},
                                      {TIFFTAG_INU_MINDISPARITY, 1, 1, TIFF_LONG, FIELD_CUSTOM, true, false, (char*)"MinDisparity"},
                                      {TIFFTAG_INU_MAXDISPARITY, 1, 1, TIFF_LONG, FIELD_CUSTOM, true, false, (char*)"MaxDisparity"},
                                      {TIFFTAG_INU_ANALOG_GAIN_LEFT, 1, 1, TIFF_FLOAT, FIELD_CUSTOM, true, false, (char*)"AnalogGainLeft"},
                                      {TIFFTAG_INU_ANALOG_GAIN_RIGHT, 1, 1, TIFF_FLOAT, FIELD_CUSTOM, true, false, (char*)"AnalogGainRight"},
                                      {TIFFTAG_INU_DIGITAL_GAIN_LEFT, 1, 1, TIFF_FLOAT, FIELD_CUSTOM, true, false, (char*)"DigitalGainLeft"},
                                      {TIFFTAG_INU_DIGITAL_GAIN_RIGHT, 1, 1, TIFF_FLOAT, FIELD_CUSTOM, true, false, (char*)"DigitalGainRight"},
                                      {TIFFTAG_INU_ISP_GAIN_LEFT, 1, 1, TIFF_FLOAT, FIELD_CUSTOM, true, false, (char*)"ISPGainLeft"},
                                      {TIFFTAG_INU_ISP_GAIN_RIGHT, 1, 1, TIFF_FLOAT, FIELD_CUSTOM, true, false, (char*)"ISPGainRight"},
                                      {TIFFTAG_INU_EXPOSURE_LEFT, 1, 1, TIFF_LONG, FIELD_CUSTOM, true, false, (char*)"ExposureLeft"},
                                      {TIFFTAG_INU_EXPOSURE_RIGHT, 1, 1, TIFF_LONG, FIELD_CUSTOM, true, false, (char*)"ExposureRight"},
                                      {TIFFTAG_INU_TIFF_VERSION_NUMBER, 1, 1, TIFF_SHORT, FIELD_CUSTOM, true, false, (char*)"TiffVersionNumber"},
                                      {TIFFTAG_INU_TIFF_OPTICAL_FOCAL_LEN_LEFTX, 1, 1, TIFF_FLOAT, FIELD_CUSTOM, true, false, (char*)"OpticalFocalLenLeftX" },
                                      {TIFFTAG_INU_TIFF_OPTICAL_FOCAL_LEN_LEFTY, 1, 1, TIFF_FLOAT, FIELD_CUSTOM, true, false, (char*)"OpticalFocalLenLeftY" },
                                      {TIFFTAG_INU_TIFF_OPTICAL_FOCAL_LEN_RIGHTX, 1, 1, TIFF_FLOAT, FIELD_CUSTOM, true, false, (char*)"OpticalFocalLenRightX" },
                                      {TIFFTAG_INU_TIFF_OPTICAL_FOCAL_LEN_RIGHTY, 1, 1, TIFF_FLOAT, FIELD_CUSTOM, true, false, (char*)"OpticalFocalLenRightY" },
                                      {TIFFTAG_INU_TIFF_OPTICAL_CENTER_LEFTX, 1, 1, TIFF_FLOAT, FIELD_CUSTOM, true, false, (char*)"OpticalCenterLeftX" },
                                      {TIFFTAG_INU_TIFF_OPTICAL_CENTER_LEFTY, 1, 1, TIFF_FLOAT, FIELD_CUSTOM, true, false, (char*)"OpticalCenterLeftY" },
                                      {TIFFTAG_INU_TIFF_OPTICAL_CENTER_RIGHTX, 1, 1, TIFF_FLOAT, FIELD_CUSTOM, true, false, (char*)"OpticalCenterRightX" },
                                      {TIFFTAG_INU_TIFF_OPTICAL_CENTER_RIGHTY, 1, 1, TIFF_FLOAT, FIELD_CUSTOM, true, false, (char*)"OpticalCenterRightY" },
                                      {TIFFTAG_INU_TIFF_OPTICAL_OFFSET, 1, 1, TIFF_FLOAT, FIELD_CUSTOM, true, false, (char*)"OpticalOffset" },
                                      {TIFFTAG_INU_TIFF_OPTICAL_BASELINE, 1, 1, TIFF_FLOAT, FIELD_CUSTOM, true, false, (char*)"OpticalBaseline" },
                                      {TIFFTAG_INU_TIFF_CALIBRATION_TEMPERATURE, 1, 1, TIFF_LONG, FIELD_CUSTOM, true, false, (char*)"CalibrationTemperature" },
                                      {TIFFTAG_INU_TIFF_DEPTH_SCALE, 1, 1, TIFF_LONG, FIELD_CUSTOM, true, false, (char*)"DepthScale" },
                                      {TIFFTAG_INU_TIFF_DEPTH_POS, 1, 1, TIFF_LONG, FIELD_CUSTOM, true, false, (char*)"DepthPosition" },
                                      {TIFFTAG_INU_TIFF_SENSOR_NUMBER_LEFT, 1, 1, TIFF_LONG, FIELD_CUSTOM, true, false, (char*)"SensorNumberLeft" },
                                      {TIFFTAG_INU_TIFF_SENSOR_NUMBER_RIGHT, 1, 1, TIFF_LONG, FIELD_CUSTOM, true, false, (char*)"SensorNumberRight" },
                                      {TIFFTAG_INU_TIFF_SENSOR_FIRST, 1, 1, TIFF_LONG, FIELD_CUSTOM, true, false, (char*)"FirstSensorNumber" },
                                      {TIFFTAG_INU_TIFF_OPTICAL_DISTORTION_LEFT, 1, 1, TIFF_FLOAT, FIELD_CUSTOM, true, false, (char*)"OpticalDistortionLeft" },
                                      {TIFFTAG_INU_TIFF_OPTICAL_DISTORTION_RIGHT, 1, 1, TIFF_FLOAT, FIELD_CUSTOM, true, false, (char*)"OpticalDistortionRight" },
                                      {TIFFTAG_INU_TIFF_OPTICAL_DISTORTION_COUNT, 1, 1, TIFF_SHORT, FIELD_CUSTOM, true, false, (char*)"OpticalDistortionCount" },
                                      {TIFFTAG_INU_TIFF_PROJECTOR_TYPE, 1, 1, TIFF_SHORT, FIELD_CUSTOM, true, false, (char*)"ProjectorType" },
                                      {TIFFTAG_INU_TIFF_DEPTH_REGISTRATION_TYPE, 1, 1, TIFF_SHORT, FIELD_CUSTOM, true, false, (char*)"DepthRegistrationType" },
    };

    TIFFMergeFieldInfo(tif, tiffFieldInfo, sizeof (tiffFieldInfo) / sizeof (tiffFieldInfo[0]));

    if (_ParentExtender != nullptr)
    {
        _ParentExtender(tif);
    }

}

void CTiffFile::Initialize()
{
    static mutex sLocker;

    if (sIsInitialized == false)
    {
        unique_lock<mutex> localLock(sLocker);// missing the name is leading to the most vexing parse problem
        if (sIsInitialized == false)
        {
            sIsInitialized = true;
            _ParentExtender = TIFFSetTagExtender(TagExtender);

            // Work in silent mode, no error or warnings are logged to stderr
            TIFFSetErrorHandler(nullptr);
            TIFFSetWarningHandler(nullptr);

            CTiffFile::FORMAT_NAME[CTiffFile::eEmpty] = "Empty";         ///< empty buffer
            CTiffFile::FORMAT_NAME[CTiffFile::eDepth] = "Depth";         ///<  Z-Buffer (16 bits per pixel)
            CTiffFile::FORMAT_NAME[CTiffFile::eRGB] = "RGB";         ///<  3 Bytes per pixel: R"; G and B
            CTiffFile::FORMAT_NAME[CTiffFile::eBGR] = "BGR";           ///<  3 Bytes per pixel: B"; G and R
            CTiffFile::FORMAT_NAME[CTiffFile::eBGRA] = "BGRA";          ///<  4 Bytes per pixel: B"; G"; R and Alpha (which is always 0)
            CTiffFile::FORMAT_NAME[CTiffFile::eDisparity] = "Disparity";     ///<  Disparities (Unsigned short)
            CTiffFile::FORMAT_NAME[CTiffFile::eRGB565] = "RGB565";        ///<  Standard RGB565 format (2 bytes per pixel)
            CTiffFile::FORMAT_NAME[CTiffFile::eRGBA] = "RGBA";        ///<  4 Bytes per pixel: R"; G"; B and Alpha (which is always 0)
            CTiffFile::FORMAT_NAME[CTiffFile::eDepthWithConfidence] = "DepthWithConfidence"; // 14 Bytes per pixel and 2 bits for confidence (LSB)
            CTiffFile::FORMAT_NAME[CTiffFile::ePointCloud] = "PointCloud"; // 14 Bytes per pixel and 2 bits for confidence (LSB)
            CTiffFile::FORMAT_NAME[CTiffFile::eYUV422] = "YUV422"; // 14 Bytes per pixel and 2 bits for confidence (LSB)
            CTiffFile::FORMAT_NAME[CTiffFile::eYUV422_B0] = "YUV422_B0"; // 14 Bytes per pixel and 2 bits for confidence (LSB)

            CTiffFile::FORMAT_NAME[CTiffFile::eYUV] = "YUV";
            CTiffFile::FORMAT_NAME[CTiffFile::eY] = "Y"; // 10 bit Y (the 11th bit indicates overflow)
            CTiffFile::FORMAT_NAME[CTiffFile::eUnpackedYUV] = "UnpackedYUV";
            CTiffFile::FORMAT_NAME[CTiffFile::eYUVFloat] = "YUVFloat";
            CTiffFile::FORMAT_NAME[CTiffFile::eBayerGRBG] = "BayerGRBG";
            CTiffFile::FORMAT_NAME[CTiffFile::eBayerRGGB] = "BayerRGGB";
            CTiffFile::FORMAT_NAME[CTiffFile::eBayerBGGR] = "BayerBGGR";
            CTiffFile::FORMAT_NAME[CTiffFile::eBayerGBRG] = "BayerGBRG";
            CTiffFile::FORMAT_NAME[CTiffFile::eDepthA0] = "DepthA0";      ///<  Depth from HW: 5 bits of confidence (MSB)
            CTiffFile::FORMAT_NAME[CTiffFile::eDepthB0] = "DepthB0";     ///<  Depth from HW: 2 bits for confidence (LSB)
            CTiffFile::FORMAT_NAME[CTiffFile::eY8] = "Y8";  // 8 bit
            CTiffFile::FORMAT_NAME[CTiffFile::eY10MSB] = "eY10MSB"; // 10 bits Y located in msb 
            CTiffFile::FORMAT_NAME[CTiffFile::eYUV_10B] = "eYUV_10B"; // 10 bits Y located in msb 
            CTiffFile::FORMAT_NAME[CTiffFile::eRAW8] = "eRAW8"; // 10 bits Y located in msb 
        }
    }

}

CTiffFile::EError CTiffFile::Write(const std::string& iOutputFileName,
                                    const unsigned char* iBuffer,
                                    unsigned int iWidth,
                                    unsigned int iHeight,
                                    ETiffFormat iFormat,
                                    const CMetaData& iMetaData,
                                    uint64_t iIndex,
                                    const std::string& iFileNameSuffix/* = std::string("")*/
                                    )
{
    Initialize();

    std::string fileName(COsUtilities::BuildFileName(iOutputFileName, TIFF_FILE_EXT, iIndex, iFileNameSuffix));

    EError ret(eOK);
#if (defined(__WIN32__) && defined(UNICODE))
    TIFF* tif = TIFFOpenW(InuCommon::COsUtilities::ConvertEncoding(fileName).c_str(), "w");
#else
    TIFF* tif = TIFFOpen(fileName.c_str(), "w");
#endif
    if (tif == nullptr)
    {
        return eFileOpen;
    }

    // Set width and height
    ret = TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, uint32(iWidth)) == 0 ? eSetField : ret;
    ret = TIFFSetField(tif, TIFFTAG_IMAGELENGTH, uint32(iHeight)) == 0 ? eSetField : ret;

    // Image format
    unsigned short bpp = (unsigned short)BytesPerPixels(iFormat);
    uint16 samplePerPixel(1), photoMetric(PHOTOMETRIC_MINISBLACK);
    switch (iFormat)
    {
	case CTiffFile::eRGB:
    case CTiffFile::eBGR:
    case CTiffFile::eBGRA:
    case CTiffFile::eRGBA:
        ret = TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8 ) == 0 ? eSetField : ret;
        photoMetric = PHOTOMETRIC_RGB;
        samplePerPixel = (iFormat == eBGR || iFormat == eRGB) ? 3 : 4;
        //ret = TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT) == 0 ? eSetField : ret;
        break;
    case CTiffFile::eDepth:
    case CTiffFile::eDepthWithConfidence:
    case CTiffFile::eDepthA0:
    case CTiffFile::eDepthB0:
    case CTiffFile::eDisparity:
    case CTiffFile::eDisparityB0:
    case CTiffFile::eYUV:
    case CTiffFile::eYUV422:
    case CTiffFile::eYUV422_B0:
    case CTiffFile::eY:
    case CTiffFile::eY8:
    case CTiffFile::eBayerGRBG:
    case CTiffFile::eBayerRGGB:
    case CTiffFile::eBayerBGGR:
    case CTiffFile::eBayerGBRG:
    case CTiffFile::eRAW8:
	case CTiffFile::eY10MSB:
	case CTiffFile::eYUV_10B:
		ret = TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8 * bpp) == 0 ? eSetField : ret;
        break;
    default:
        break;
    }

    ret = TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE) == 0 ? eSetField : ret;

    ret = TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photoMetric) == 0 ? eSetField : ret;

    // Defines that data channels are not separated (RGBARGBA...)
    ret = TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB) == 0 ? eSetField : ret;
    ret = TIFFSetField(tif, TIFFTAG_IMAGEDESCRIPTION, FORMAT_NAME[iFormat].c_str()) == 0 ? eSetField : ret;
    ret = TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT) == 0 ? eSetField : ret;
    ret = TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, samplePerPixel) == 0 ? eSetField : ret;
    ret = TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 1) == 0 ? eSetField : ret;
    ret = TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG) == 0 ? eSetField : ret;
    ret = TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_CENTIMETER) == 0 ? eSetField : ret;

    string moduleName=COsUtilities::GetExeFileName();
    ret = TIFFSetField(tif, TIFFTAG_SOFTWARE, moduleName.c_str()) == 0 ? eSetField : ret;

    ret = TIFFSetField(tif, TIFFTAG_ARTIST, INUITIVE_VENDOR_NAME.c_str()) == 0 ? eSetField : ret;

    // Set version 2
    ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_VERSION_NUMBER, CURRENT_VERSION_NUMBER)  == 0 ? eSetField : ret;

    // Write MetaData
    if (iMetaData.SensorID.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_SENSORID,  iMetaData.SensorID.Get().c_str()) == 0 ? eSetField : ret;
    }
    if (iMetaData.SensorName.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_SENSORNAME,  iMetaData.SensorName.Get().c_str()) == 0 ? eSetField : ret;
    }
    if (iMetaData.CalibrationRevision.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_REVISION,  iMetaData.CalibrationRevision.Get()) == 0 ? eSetField : ret;
    }
    if (iMetaData.SensorResolution.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_RESOLUTION,  iMetaData.SensorResolution.Get()) == 0 ? eSetField : ret;
    }
    if (iMetaData.InputType.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_INPUTTYPE,  iMetaData.InputType.Get()) == 0 ? eSetField : ret;
    }

    ret = TIFFSetField(tif, TIFFTAG_INU_FORMAT, iFormat) == 0 ? eSetField : ret;

    if (iMetaData.AddtionalData.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_ADDIONAL_DATA,  iMetaData.AddtionalData.Get().c_str()) == 0 ? eSetField : ret;
    }

    if (iMetaData.SensorModel.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_SENSOR_MODEL,  iMetaData.SensorModel.Get().c_str()) == 0 ? eSetField : ret;
    }

    if (iMetaData.HWType.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_HW_TYPE,  iMetaData.HWType.Get()) == 0 ? eSetField : ret;
    }

    if (iMetaData.DisparityOffset.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_DISPARITY_OFFSET,  iMetaData.DisparityOffset.Get()) == 0 ? eSetField : ret;
    }

    if (iMetaData.FrameIndex.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_FRAME_INDEX,  iMetaData.FrameIndex.Get()) == 0 ? eSetField : ret;
    }

    if (iMetaData.Timestamp.Valid())
    {
        uint32_t high(uint32_t(iMetaData.Timestamp.Get() >> 32)), low(uint32_t(iMetaData.Timestamp.Get()));
        ret = TIFFSetField(tif, TIFFTAG_INU_TIMESTAMP_HIGH,  high) == 0 ? eSetField : ret;
        ret = TIFFSetField(tif, TIFFTAG_INU_TIMESTAMP_LOW,  low) == 0 ? eSetField : ret;
    }

    if (iMetaData.MinDisparity.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_MINDISPARITY,  iMetaData.MinDisparity.Get()) == 0 ? eSetField : ret;
    }

    if (iMetaData.MaxDisparity.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_MAXDISPARITY,  iMetaData.MaxDisparity.Get()) == 0 ? eSetField : ret;
    }

    if (iMetaData.AnaglogGain.ValidFirst() && (std::abs(iMetaData.AnaglogGain.Get().first) > FLT_EPSILON) )
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_ANALOG_GAIN_LEFT,  iMetaData.AnaglogGain.Get().first) == 0 ? eSetField : ret;
    }

    if (iMetaData.AnaglogGain.ValidSecond() && (std::abs(iMetaData.AnaglogGain.Get().second) > FLT_EPSILON))
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_ANALOG_GAIN_RIGHT, iMetaData.AnaglogGain.Get().second) == 0 ? eSetField : ret;
    }

    if (iMetaData.DigitalGain.ValidFirst() && (std::abs(iMetaData.DigitalGain.Get().first) > FLT_EPSILON) )
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_DIGITAL_GAIN_LEFT,  iMetaData.DigitalGain.Get().first) == 0 ? eSetField : ret;
    }

    if (iMetaData.DigitalGain.ValidSecond() && (std::abs(iMetaData.DigitalGain.Get().second) > FLT_EPSILON))
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_DIGITAL_GAIN_RIGHT, iMetaData.DigitalGain.Get().second) == 0 ? eSetField : ret;
    }

    if (iMetaData.IspGain.ValidFirst() && (std::abs(iMetaData.IspGain.Get().first) > FLT_EPSILON) )
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_ISP_GAIN_LEFT, iMetaData.IspGain.Get().first) == 0 ? eSetField : ret;
    }

    if (iMetaData.IspGain.ValidSecond() && (std::abs(iMetaData.IspGain.Get().second) > FLT_EPSILON))
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_ISP_GAIN_RIGHT, iMetaData.IspGain.Get().second) == 0 ? eSetField : ret;
    }


    if (iMetaData.ExposureTime.ValidFirst() && (std::abs(int(iMetaData.ExposureTime.Get().first)) > 0) )
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_EXPOSURE_LEFT,  iMetaData.ExposureTime.Get().first) == 0 ? eSetField : ret;
    }

    if (iMetaData.ExposureTime.ValidSecond() && (std::abs(int(iMetaData.ExposureTime.Get().second)) > 0))
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_EXPOSURE_RIGHT, iMetaData.ExposureTime.Get().second) == 0 ? eSetField : ret;
    }

    if (iMetaData.ProjectorType.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_PROJECTOR_TYPE, iMetaData.ProjectorType.Get()) == 0 ? eSetField : ret;
    }

    if (iMetaData.OpticalBaseline.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_OPTICAL_BASELINE, iMetaData.OpticalBaseline.Get()) == 0 ? eSetField : ret;
    }


    if (iMetaData.OpticalFocalLengthLeft.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_OPTICAL_FOCAL_LEN_LEFTX, iMetaData.OpticalFocalLengthLeft.Get().first) == 0 ? eSetField : ret;
        ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_OPTICAL_FOCAL_LEN_LEFTY, iMetaData.OpticalFocalLengthLeft.Get().second) == 0 ? eSetField : ret;
    }

    if (iMetaData.OpticalFocalLengthRight.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_OPTICAL_FOCAL_LEN_RIGHTX, iMetaData.OpticalFocalLengthRight.Get().first) == 0 ? eSetField : ret;
        ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_OPTICAL_FOCAL_LEN_RIGHTY, iMetaData.OpticalFocalLengthRight.Get().second) == 0 ? eSetField : ret;
    }

    if (iMetaData.OpticalCenterLeft.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_OPTICAL_CENTER_LEFTX, iMetaData.OpticalCenterLeft.Get().first) == 0 ? eSetField : ret;
        ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_OPTICAL_CENTER_LEFTY, iMetaData.OpticalCenterLeft.Get().second) == 0 ? eSetField : ret;
    }

    if (iMetaData.OpticalCenterRight.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_OPTICAL_CENTER_RIGHTX, iMetaData.OpticalCenterRight.Get().first) == 0 ? eSetField : ret;
        ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_OPTICAL_CENTER_RIGHTY, iMetaData.OpticalCenterRight.Get().second) == 0 ? eSetField : ret;
    }

    ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_OPTICAL_DISTORTION_RIGHT, iMetaData.OpticalCenterRight.Get().first) == 0 ? eSetField : ret;
    ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_OPTICAL_DISTORTION_LEFT, iMetaData.OpticalCenterRight.Get().second) == 0 ? eSetField : ret;

    if (iMetaData.CalibrationTemparature.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_CALIBRATION_TEMPERATURE, iMetaData.CalibrationTemparature.Get()) == 0 ? eSetField : ret;
    }

    if (iMetaData.DepthScale.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_DEPTH_SCALE, iMetaData.DepthScale.Get()) == 0 ? eSetField : ret;
    }

    if (iMetaData.DepthPosition.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_DEPTH_POS, iMetaData.DepthPosition.Get()) == 0 ? eSetField : ret;
    }

    if (iMetaData.SensorNumber.ValidFirst())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_SENSOR_NUMBER_LEFT, iMetaData.SensorNumber.Get().first) == 0 ? eSetField : ret;
    }

    if (iMetaData.SensorNumber.ValidSecond())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_SENSOR_NUMBER_RIGHT, iMetaData.SensorNumber.Get().second) == 0 ? eSetField : ret;
    }

    if (iMetaData.FirstSensorNumber.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_SENSOR_FIRST, iMetaData.FirstSensorNumber.Get()) == 0 ? eSetField : ret;
    }

    if (iMetaData.DepthRegistration.Valid())
    {
        ret = TIFFSetField(tif, TIFFTAG_INU_TIFF_DEPTH_REGISTRATION_TYPE, iMetaData.DepthRegistration.Get()) == 0 ? eSetField : ret;
    }

    if (ret == CTiffFile::eOK)
    {
        // Write the image
        int offset = 0;

        for (unsigned int i = 0; i < iHeight; i++)
        {
            if (TIFFWriteScanline(tif, (void*)(iBuffer + offset), i) == -1)
            {
                ret = eWriteData;
                break;
            }
            offset += iWidth * bpp;
        }
    }

    TIFFClose(tif);

    return ret;
}

CTiffFile::EError CTiffFile::ReadHeader(const std::string& iFileName,
                                        unsigned int& oWidth,
                                        unsigned int& oHeight,
                                        unsigned int& oBytesPerPixel,
                                        ETiffFormat& oFormat,
                                        CMetaData& oMetaData)
{
    Initialize();

#if (defined(__WIN32__) && defined(UNICODE))
    TIFF* tif = TIFFOpenW(InuCommon::COsUtilities::ConvertEncoding(iFileName).c_str(), "r");
#else
    TIFF* tif = TIFFOpen(iFileName.c_str(), "r");
#endif
	oFormat = eUnRecognizedFormat;

    if (tif == nullptr)
    {
        if (IsDeprecatedRawFile(iFileName))
        {
            oFormat = eDeprecatedRawFiles;
            // Reset unknown data
            oWidth = oHeight = oBytesPerPixel = UINT_MAX;
            oMetaData = CMetaData();
            return eOK;
        }
        return eFileOpen;
    }

    EError ret = eOK;

    // Read tif version
    uint16 versionNumber(0);
    TIFFGetField(tif, TIFFTAG_INU_TIFF_VERSION_NUMBER, &versionNumber);

    // Set width and height
    if (ret == CTiffFile::eOK)
    {
        uint32 width(0), height(0);
        ret = TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width) == 0 ? eGetField : ret;
        ret = TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height) == 0 ? eGetField : ret;
        oWidth = width;
        oHeight = height;
    }

    // Find file format
    oFormat = eUnRecognizedFormat;
    if (ret == CTiffFile::eOK)
    {
        uint16 format(0); 
        ret = TIFFGetField(tif, TIFFTAG_INU_FORMAT, &format) == 0 ? eGetField : ret;
        if (ret == CTiffFile::eOK)
        {
            oFormat = ETiffFormat(format);
        }
        else
        {
            ret = TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &format) == 0 ? eGetField : ret;
            if (ret == eOK && format == PHOTOMETRIC_RGB)
            {
                uint16 bytesPerSample(3);
                uint8* imageDescription = nullptr;
                string description;
                if (TIFFGetField(tif, TIFFTAG_IMAGEDESCRIPTION, &imageDescription) != 0 && imageDescription != nullptr)
                {
                    description = string(reinterpret_cast<char*>(imageDescription));
                }

                if (TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &bytesPerSample) == 0)
                {
                    bytesPerSample = 3;
                    if (description == FORMAT_NAME[eRGB])
                    {
                        oFormat = eRGB;
                    }
                    else
                    {
                        oFormat = eBGR;
                    }
                }
                if (bytesPerSample == 4)
                {
                    if (description == FORMAT_NAME[eRGBA])
                    {
                        oFormat = eRGBA;
                    }
                    else
                    {
                        oFormat = eBGRA;
                    }
                }
                else
                {
                    ret = eUnexpectedFormat;
                }
            }
        }

        if (oFormat == eUnRecognizedFormat)
        {
            uint16 bpp(0), bytesPerSample(1);
            ret = TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bpp) == 0 ? eGetField : ret;
            oBytesPerPixel = bpp / 8;
            if (TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &bytesPerSample) != 0)
            {
                oBytesPerPixel *= bytesPerSample;
            }
        }
        else
        {
            oBytesPerPixel = BytesPerPixels(oFormat);
        }
    }

    // Read Metadata
    oMetaData = CMetaData();

    uint8* buffer=nullptr;
    if (TIFFGetField(tif, TIFFTAG_INU_SENSORID, &buffer) == 1 && buffer != nullptr)
    {
        oMetaData.SensorID.Set(reinterpret_cast<char*>(buffer));
    }
    buffer=nullptr;
    if (TIFFGetField(tif, TIFFTAG_INU_SENSORNAME, &buffer) == 1 && buffer != nullptr)
    {
        oMetaData.SensorName.Set(reinterpret_cast<char*>(buffer));
    }
    if (TIFFGetField(tif, TIFFTAG_INU_ADDIONAL_DATA, &buffer) == 1 && buffer != nullptr)
    {
        oMetaData.AddtionalData.Set(reinterpret_cast<char*>(buffer));
    }

    uint16 tmp(0);
    if (TIFFGetField(tif, TIFFTAG_INU_RESOLUTION, &tmp) == 1)
    {
        oMetaData.SensorResolution.Set(tmp < eNumOfResolutions ? ESensorResolution(tmp) : eUnknownResolution);
    }
    if (TIFFGetField(tif, TIFFTAG_INU_REVISION, &tmp) == 1)
    {
        oMetaData.CalibrationRevision.Set(tmp);
    }
    if (TIFFGetField(tif, TIFFTAG_INU_INPUTTYPE, &tmp) == 1)
    {
        oMetaData.InputType.Set(tmp < eUnknownType ? EInputType(tmp) : eUnknownType);
    }


    if (TIFFGetField(tif, TIFFTAG_INU_SENSOR_MODEL, &buffer) == 1)
    {
        oMetaData.SensorModel.Set(reinterpret_cast<char*>(buffer));
    }

    if (TIFFGetField(tif, TIFFTAG_INU_HW_TYPE, &tmp) == 1)
    {
        oMetaData.HWType.Set(EHWType(tmp));
    }

    uint32_t tmp32;
    if (TIFFGetField(tif, TIFFTAG_INU_DISPARITY_OFFSET, &tmp32) == 1)
    {
        oMetaData.DisparityOffset.Set(tmp32);
    }

    if (TIFFGetField(tif, TIFFTAG_INU_FRAME_INDEX, &tmp32) == 1)
    {
        oMetaData.FrameIndex.Set(tmp32);
    }
    uint32_t high, low;
    if (TIFFGetField(tif, TIFFTAG_INU_TIMESTAMP_HIGH, &high) == 1 && TIFFGetField(tif, TIFFTAG_INU_TIMESTAMP_LOW, &low) == 1)
    {
        uint64_t tmp64(high);
        tmp64 = (tmp64 << 32) | low;
        oMetaData.Timestamp.Set(tmp64);
    }

    if (TIFFGetField(tif, TIFFTAG_INU_MINDISPARITY, &tmp32) == 1)
    {
        oMetaData.MinDisparity.Set(tmp32);
    }
    if (TIFFGetField(tif, TIFFTAG_INU_MAXDISPARITY, &tmp32) == 1)
    {
        oMetaData.MaxDisparity.Set(tmp32);
    }

    if (versionNumber > 12)
    {
        float leftf(0), rightf(0);
        if (TIFFGetField(tif, TIFFTAG_INU_ANALOG_GAIN_LEFT, &leftf) == 1)
        {
            oMetaData.AnaglogGain.SetFirst(leftf);
        }
        if (TIFFGetField(tif, TIFFTAG_INU_ANALOG_GAIN_RIGHT, &rightf) == 1)
        {
            oMetaData.AnaglogGain.SetSecond(rightf);
        }
        if (TIFFGetField(tif, TIFFTAG_INU_DIGITAL_GAIN_LEFT, &leftf) == 1)
        {
            oMetaData.DigitalGain.SetFirst(leftf);
        }
        if (TIFFGetField(tif, TIFFTAG_INU_DIGITAL_GAIN_RIGHT, &rightf) == 1)
        {
            oMetaData.DigitalGain.SetSecond(rightf);
        }
        if (versionNumber > 15)
        {
            if (TIFFGetField(tif, TIFFTAG_INU_ISP_GAIN_LEFT, &leftf) == 1)
            {
                oMetaData.IspGain.SetFirst(leftf);
            }
            if (TIFFGetField(tif, TIFFTAG_INU_ISP_GAIN_RIGHT, &rightf) == 1)
            {
                oMetaData.IspGain.SetSecond(rightf);
            }
        }
    }
    else
    {
        // In older version the Gain is saved as integer, 
        // there is a need to change the defintion in tiffFieldInfo in oder to read it 
    }

    uint32_t left(0), right(0);
    if (TIFFGetField(tif, TIFFTAG_INU_EXPOSURE_LEFT, &left) == 1)
    {
        oMetaData.ExposureTime.SetFirst(left);
    }
    if (TIFFGetField(tif, TIFFTAG_INU_EXPOSURE_RIGHT, &right) == 1)
    {
        oMetaData.ExposureTime.SetSecond(right);
    }

    if (versionNumber > 13)
    {
        if (TIFFGetField(tif, TIFFTAG_INU_TIFF_PROJECTOR_TYPE, &tmp) == 1)
        {
            oMetaData.ProjectorType.Set(EProjectors(tmp));
        }
    }

    if (versionNumber > 14)
    {
        if (TIFFGetField(tif, TIFFTAG_INU_TIFF_DEPTH_REGISTRATION_TYPE, &tmp) == 1)
        {
            oMetaData.DepthRegistration.Set(EDepthRegistrationType(tmp));
        }
    }

    float x, y, tmpf;

    if (TIFFGetField(tif, TIFFTAG_INU_TIFF_OPTICAL_FOCAL_LEN_LEFTX, &x) == 1 && TIFFGetField(tif, TIFFTAG_INU_TIFF_OPTICAL_FOCAL_LEN_LEFTY, &y) == 1)
    {
        oMetaData.OpticalFocalLengthLeft.Set(make_pair(x, y));
    }
    if (TIFFGetField(tif, TIFFTAG_INU_TIFF_OPTICAL_FOCAL_LEN_RIGHTX, &x) == 1 && TIFFGetField(tif, TIFFTAG_INU_TIFF_OPTICAL_FOCAL_LEN_RIGHTY, &y) == 1)
    {
        oMetaData.OpticalFocalLengthRight.Set(make_pair(x, y));
    }
    if (TIFFGetField(tif, TIFFTAG_INU_TIFF_OPTICAL_CENTER_LEFTX, &x) == 1 && TIFFGetField(tif, TIFFTAG_INU_TIFF_OPTICAL_CENTER_LEFTY, &y) == 1)
    {
        oMetaData.OpticalCenterLeft.Set(make_pair(x, y));
    }
    if (TIFFGetField(tif, TIFFTAG_INU_TIFF_OPTICAL_CENTER_RIGHTX, &x) == 1 && TIFFGetField(tif, TIFFTAG_INU_TIFF_OPTICAL_CENTER_RIGHTY, &y) == 1)
    {
        oMetaData.OpticalCenterRight.Set(make_pair(x, y));
    }
    if (TIFFGetField(tif, TIFFTAG_INU_TIFF_OPTICAL_BASELINE, &tmpf) == 1)
    {
        oMetaData.OpticalBaseline.Set(tmpf);
    }


    uint32_t temperature;
    if (TIFFGetField(tif, TIFFTAG_INU_TIFF_CALIBRATION_TEMPERATURE, &temperature) == 1)
    {
        oMetaData.CalibrationTemparature.Set(temperature);
    }


    uint32_t pos, scale;
    if (TIFFGetField(tif, TIFFTAG_INU_TIFF_DEPTH_SCALE, &scale) == 1)
    {
        oMetaData.DepthScale.Set(scale);
    }
    if (TIFFGetField(tif, TIFFTAG_INU_TIFF_DEPTH_POS, &pos) == 1)
    {
        oMetaData.DepthPosition.Set(pos);
    }


    if (TIFFGetField(tif, TIFFTAG_INU_TIFF_SENSOR_NUMBER_LEFT, &tmp32) == 1)
    {
        oMetaData.SensorNumber.SetFirst(tmp32);
    }
    if (TIFFGetField(tif, TIFFTAG_INU_TIFF_SENSOR_NUMBER_RIGHT, &tmp32) == 1)
    {
        oMetaData.SensorNumber.SetSecond(tmp32);
    }

    if (TIFFGetField(tif, TIFFTAG_INU_TIFF_SENSOR_FIRST, &tmp32) == 1)
    {
        oMetaData.FirstSensorNumber.Set(tmp32);
    }

    if ( (TIFFGetField(tif, TIFFTAG_INU_TIFF_OPTICAL_DISTORTION_RIGHT, &x) == 1) &&
         (TIFFGetField(tif, TIFFTAG_INU_TIFF_OPTICAL_DISTORTION_LEFT, &y) == 1) )
    {
        oMetaData.OpticalCenterRight.Set(std::make_pair(x,y));
    }

    // Check that this file is Inuitive's
    buffer = nullptr;
    auto tmpRes =  TIFFGetField(tif, TIFFTAG_ARTIST, &buffer) == 0 ? eGetField : ret;
    if (tmpRes == eOK)
    {
        oMetaData.TiffVendor.Set(string(reinterpret_cast<char*>(buffer)));
    }
    //_TIFFfree(buffer);  Free or delete cause to crash in destruction

    TIFFClose(tif);

    return ret;
}


CTiffFile::EError CTiffFile::ReadData(const std::string& iFileName,
                                      unsigned char*& oBuffer,
                                      unsigned int iWidth,
                                      unsigned int iHeight,
                                      unsigned int iBytesPerPixel)
{
    Initialize();

    EError ret(eOK);

#if (defined(__WIN32__) && defined(UNICODE))
    TIFF* tif = TIFFOpenW(InuCommon::COsUtilities::ConvertEncoding(iFileName).c_str(), "r");
#else
    TIFF* tif = TIFFOpen(iFileName.c_str(), "r");
#endif

    if (tif == nullptr)
    {
        if (IsDeprecatedRawFile(iFileName))
        {
            ifstream ifs;
            ifs.open(InuCommon::COsUtilities::ConvertEncoding(iFileName).c_str(), std::ifstream::in | std::ifstream::binary);
            if (ifs.good())
            {
                unsigned int size = iWidth*iHeight*iBytesPerPixel;
                if (oBuffer == nullptr)
                {
                    size=(unsigned int)(boost::filesystem::file_size(iFileName));
                    oBuffer = new unsigned char[size];
                }

                ifs.read((char*)oBuffer, size);

                ret = ifs.good() ? eOK : eReadData;
            }
            else
            {
                ret = eFileOpen;
            }

            ifs.close();
        }
        else
        {
            ret = eFileOpen;
        }
    }
    else
    {
        if (oBuffer == nullptr)
        {
            oBuffer = new unsigned char[iWidth*iHeight*iBytesPerPixel];
        }

        int offset = 0;

        unsigned int rowSize = (unsigned int)(TIFFScanlineSize(tif));
        if ((iWidth * iBytesPerPixel) == rowSize)
        {
            for (unsigned int i = 0; i < iHeight; i++)
            {
                if (TIFFReadScanline(tif, (void*)(oBuffer + offset), i) == -1)
                {
                    ret = eReadData;
                    break;
                }
                offset += rowSize;
            }

            TIFFClose(tif);
        }
        else
        {
            TIFFClose(tif);

            // Bug in Inuitive's old tif file, data should be read by using fopen
            ifstream ifs;
            ifs.open(InuCommon::COsUtilities::ConvertEncoding(iFileName).c_str(), std::ifstream::in | std::ifstream::binary);
            if (ifs.good())
            {
                unsigned int size = iWidth*iHeight*iBytesPerPixel;
                if (oBuffer == nullptr)
                {
                    size=(unsigned int)(boost::filesystem::file_size(iFileName));
                    oBuffer = new unsigned char[size];
                }

                ifs.seekg(8);

                ifs.read((char*)oBuffer, size);

                ret = ifs.good() ? eOK : eReadData;
            }
        }
    }

    return ret;
}

CTiffFile::EError CTiffFile::Read(const std::string& iFileName,
                                  unsigned char*& oBuffer,
                                  unsigned int& oWidth,
                                  unsigned int& oHeight,
                                  unsigned int& oBytesPerPixel,
                                  ETiffFormat& oFormat,
                                  CMetaData& oMetaData)
{
    CTiffFile::EError ret = ReadHeader(iFileName, oWidth, oHeight, oBytesPerPixel, oFormat, oMetaData);
    if (ret == eOK)
    {
        ret = ReadData(iFileName, oBuffer, oWidth , oHeight, oBytesPerPixel);
    }
    return ret;
}


bool CTiffFile::IsDeprecatedRawFile(const std::string& iFileName)
{
    // Backward compatibility - support old raw formats
    size_t dotIndex = iFileName.find_last_of('.');
    if (dotIndex != string::npos)
    {
        string extension = iFileName.substr(dotIndex+1, iFileName.length() - dotIndex - 1);
        if (extension == "raw")
        {
            return true;
        }
    }
    return false;
}

unsigned int InuCommon::CTiffFile::BytesPerPixels(ETiffFormat inputFormat)
{
    switch (inputFormat)
    {
    case eEmpty:
        return 0;

    case eY8:
    case eRAW8:
        return 1;

    case eY:
	case eY10MSB:
	case eDisparity:
    case eDisparityB0:
    case eDepth:
    case eYUV:
    case eDepthA0:
    case eDepthB0:
    case eDepthWithConfidence:
    case eRGB565:
    case eBayerGRBG:
    case eBayerRGGB:
    case eBayerBGGR:
    case eBayerGBRG:
    case eYUV422:
    case eYUV422_B0:
		 return 2;

    case eYUVFloat:
        return 3 * sizeof(float);
    case eBGR:
    case eRGB:
    case eUnpackedYUV:
	case eYUV_10B:
        return 3;

    case eBGRA:
    case eRGBA:
    case eABGR:
    case eARGB:
        return 4;

    case ePointCloud:
        return sizeof(float);  //float X Y Z

    default:
        return 0;
    }
}
