//
// File : TiffFile.h
//

#ifndef __TIFFFILE_H__
#define __TIFFFILE_H__


#include <string>
#include <map>
#include <limits>

#include "CommonUtilitiesDefs.h"
#include "stdint.h"

typedef struct tiff TIFF;
#undef min
#undef max

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#if _MSC_VER < 1900
#include <stdint.h>
#endif
#endif

namespace InuCommon
{

    ////////////////////////////////////////////////////////////////////////
    /// \brief    Interface for Tiff files
    ///
    /// Role: Utility class which provides interface for reading and writing
    ///       Inuitive's images in Tiff files format.
    ///
    /// Responsibilities:
    ///      1. Pack and save Inuitive's image as Tiff file
    ///      2. Read header, data or both
    ////////////////////////////////////////////////////////////////////////
    class COMMONUTILITIES_API CTiffFile
    {

    public:

        /// \brief    All supported formats
        enum ETiffFormat
        {
            eEmpty = 0,     ///<  Empty buffer
            eDepth,         ///<  Z-Buffer (16 bits per pixel)
            eBGR,           ///<  3 Bytes per pixel: B, G and R
            eBGRA,          ///<  4 Bytes per pixel: B, G, R and Alpha (which is always 0xff - 100%)
            eDisparity,     ///<  2 Bytes per pixel: 4 MSB for confidence and 12 LSB for Disparities
            eRGB565,        ///<  Standard RGB565 format (2 bytes per pixel)
            eDisparityB0,   ///<  2 Bytes per pixel: 4 LSB for confidence and 4 LSB for Disparities
            eRGBA,          ///<  4 Bytes per pixel: R, G, B and Alpha (which is always 0xff - 100%)
            eDepthWithConfidence, ///<  Z-Buffer: 14 MSBits are depth and 2 LSBits are for confidence (The higher the better)
            ePointCloud,    ///<  XYZ float point cloud buffer
            eYUV422,        ///<  2 bytes of compressed Y, U, V
            eABGR,
            eARGB,
            eRGB,           ///<  3 Bytes per pixel: R, G and B
            ePointCloudWithConfidence,
            eYUV422_B0,        ///<  2 bytes of compressed Y, U, V

            eYUV = eEmpty+100,
            eY,             ///< 10 bit Y
            eUnpackedYUV,
            eYUVFloat,
            eBayerGRBG,
            eBayerRGGB,
            eBayerBGGR,
            eBayerGBRG,
            eDepthA0,       ///<  Depth from HW: 5 bits of confidence (MSB)
            eDepthB0,       ///<  Depth from HW: 2 bits for confidence (LSB)
            eY8,            ///< 8 bit Y

            eDeprecatedRawFiles = eEmpty+200,
            eUnRecognizedFormat,

			eY10MSB = eEmpty + 300, ///<  2 Bytes per pixel: 10 bits Y located in msb
			eYUV_10B,
            eRAW8,

            eNumOfFormats
        };

        /// \brief    Returned error codes
        enum EError
        {
            eOK = 0,
            eFileOpen,          /// Can't open input tiff file
            eSetField,          /// Error in writing one of header's attribute
            eWriteData,         /// Error in writing image buffer
            eReadData,          /// Error in reading image buffer
            eGetField,          /// Missing field in input tif file
            eVendor,            /// The input file was not created by Inuitive
            eVersionNum,        /// Unexpected version
            eUnexpectedFormat   /// Input file format is different than ETiffFormat
        };

        /// \brief  Different sensor pre-defined resolutions
        enum ESensorResolution
        {
            eUnknownResolution = 0,
            eBinning = 1,
            eVerticalBinning,
            eFull,
            eAlternate,
            eNumOfResolutions
        };

        /// \brief  Different image input types
        enum EInputType
        {
            eLeft = 0,
            eRight,
            eInterleaved,
            eUnknownType
        };

        /// \brief  Head tracking source type
        enum EHTSource
        {
            eVideo = 0,
            eWebcam
        };

        /// \brief  Head tracking source type
        enum EDepthRegistrationType
        {
            eNoDepthRegistration = 0,
            eHWDepthRegistration,
            eSDKDepthRegistration,
        };

        /// \brief  Different HW Type
        enum EHWType
        {
            eA0 = 0,
            eB0,
            eNU4000FPGA,
            eNU4000A0,
            eNU4000B0,
            eNU4000C0,
            eNU4000C1,
            eNU4100A0,
            eUnknown=100
        };

        enum EProjectors
        {
            ePatterns = 0,  ///< Main projector, Projecting Patterns
            eFlood = 1,     ///< Projecting Flood
            eNumOfProjectors = 100
        };

        struct CMetaData;

        template<class T>
        class CMetaDataAttribute
        {
            friend struct CMetaData;
        public:
            bool Valid() const { return mValue != mDefault; }
            const T& Get() const { return mValue; }
            void Set(const T& input) { mValue = input; }
        private:
            CMetaDataAttribute(const T& iDefault) : mDefault(iDefault), mValue(iDefault) { }
            T  mDefault;
            T  mValue;
        };

        template<class T>
        class CMetaDataPair
        {
            friend struct CMetaData;
        public:
            bool ValidFirst() const { return mValue.first != mDefault.first; }
            bool ValidSecond() const { return  mValue.second != mDefault.second; }

            void SetFirst(T input) { mValue.first = input; }
            void SetSecond(T input) { mValue.second = input; }

            const std::pair<T, T>& Get() const { return mValue; }

        private:
            CMetaDataPair(T iDefault) : mDefault(std::make_pair(iDefault, iDefault)), mValue(std::make_pair(iDefault, iDefault)) { }
            std::pair<T, T>  mDefault;
            std::pair<T, T>  mValue;
        };

        /// \brief    Additional attributes that can be saved in the tiff file together with image data
        struct CMetaData
        {
            CMetaData() :
                SensorID(""),
                SensorNumber(std::numeric_limits<uint32_t>::max()),
                FirstSensorNumber(std::numeric_limits<uint16_t>::max()),
                SensorName(""),
                CalibrationRevision(std::numeric_limits<short>::max()),
                SensorResolution(eUnknownResolution),
                InputType(eUnknownType),
                AddtionalData(""),
                SensorModel(""),
                HWType(eUnknown),
                DisparityOffset(0),
                FrameIndex(std::numeric_limits<uint32_t>::max()),
                Timestamp(std::numeric_limits<uint64_t>::max()),
                MinDisparity(std::numeric_limits<uint32_t>::max()),
                MaxDisparity(std::numeric_limits<uint32_t>::max()),
                AnaglogGain(std::numeric_limits<float>::max()),
                DigitalGain(std::numeric_limits<float>::max()),
                IspGain(std::numeric_limits<float>::max()),
                ExposureTime(std::numeric_limits<uint32_t>::max()),
                ProjectorType(eNumOfProjectors),
                OpticalFocalLengthLeft(std::make_pair(std::numeric_limits<float>::max(), std::numeric_limits<float>::max())),
                OpticalFocalLengthRight(std::make_pair(std::numeric_limits<float>::max(), std::numeric_limits<float>::max())),
                OpticalCenterLeft(std::make_pair(std::numeric_limits<float>::max(), std::numeric_limits<float>::max())),
                OpticalCenterRight(std::make_pair(std::numeric_limits<float>::max(), std::numeric_limits<float>::max())),
                OpticalBaseline(std::numeric_limits<float>::max()),
                CalibrationTemparature(0),
                DepthScale(std::numeric_limits<uint32_t>::max()),
                DepthPosition(std::numeric_limits<uint32_t>::max()),
                TiffVendor(""),
                DepthRegistration(eNoDepthRegistration)
            {
            }

            /// \brief    Unique Sensor ID as is defined by the FW
            CMetaDataAttribute<std::string> SensorID;

            /// \brief    Unique Sensor ID as is defined by the FW (left is first and right is the second)
            CMetaDataPair<uint32_t> SensorNumber;

            // \brief    The SensorID that its data appears first in interleaved frame
            CMetaDataAttribute<uint32_t> FirstSensorNumber;
            
            /// \brief    Logical name which was given by the calibration process
            CMetaDataAttribute<std::string> SensorName;

            /// \brief    Sensor calibration revision number 
            CMetaDataAttribute<uint16_t> CalibrationRevision;

            /// \brief    Sensor resolution which was used while creating this image
            CMetaDataAttribute<ESensorResolution> SensorResolution;

            /// \brief    Image represents interleaved frame (relevant only to eRGB or eYUVPacked formats)
            CMetaDataAttribute<EInputType> InputType;

            /// \brief    Free text (for serialized user defined attributes)
            CMetaDataAttribute<std::string> AddtionalData;

            /// \brief    Sensor model
            CMetaDataAttribute<std::string> SensorModel;

            /// \brief    Sensor Type A0/B0
            CMetaDataAttribute<EHWType> HWType;

            /// \brief    Sensor Type A0/B0
            CMetaDataAttribute<uint16_t> DisparityOffset;

            /// \brief    Frame index as is set by FW
            CMetaDataAttribute<uint32_t> FrameIndex;

            /// \brief    Frame acquisition time stamp
            CMetaDataAttribute<uint64_t> Timestamp;

            /// \brief    Frame Min Disparity
            CMetaDataAttribute<uint32_t> MinDisparity;

            /// \brief    Frame Max Disparity
            CMetaDataAttribute<uint32_t> MaxDisparity;

            /// \brief    Analog Gain (left,right)
            CMetaDataPair<float> AnaglogGain;

            /// \brief    Digital Gain (left,right)
            CMetaDataPair <float> DigitalGain;

            /// \brief    ISP Gain (left,right)
            CMetaDataPair<float> IspGain;

            /// \brief    Exposure (left,right)
            CMetaDataPair<uint32_t > ExposureTime;

            /// \brief    Projector Type 
            CMetaDataAttribute<EProjectors>  ProjectorType;

            /// \brief    Optical Focal Length (left,right)
            CMetaDataAttribute<std::pair<double, double>> OpticalFocalLengthLeft;

            /// \brief    Optical Focal Length (left,right)
            CMetaDataAttribute<std::pair<double, double>> OpticalFocalLengthRight;

            /// \brief    Optical Center (left,right)
            CMetaDataAttribute<std::pair<double, double>> OpticalCenterLeft;

            /// \brief    Optical Center (left,right)
            CMetaDataAttribute<std::pair<double, double>> OpticalCenterRight;

            /// \brief    Optical Baseline
            CMetaDataAttribute<double> OpticalBaseline;

            /// \brief    Sensor calibration revision number
            CMetaDataAttribute<uint16_t> CalibrationTemparature;

            /// \brief    Optical Baseline
            CMetaDataAttribute<uint32_t> DepthScale;

            /// \brief    Depth representation start from this bit to MSB
            CMetaDataAttribute<uint32_t> DepthPosition;

            /// \brief    Unique Sensor ID as is defined by the FW
            CMetaDataAttribute<std::string> TiffVendor;

            /// \brief    Unique Sensor ID as is defined by the FW
            CMetaDataAttribute<EDepthRegistrationType> DepthRegistration;
        };


       /// \brief    Default extension of Tiff files
       static const std::string  TIFF_FILE_EXT;


        /// \brief    Save input image data as Tiff file
        /// \param[in] iFileName    Output file name.
        /// \param[in] iBuffer    Data buffer.
        /// \param[in] iWidth
        /// \param[in] iHeight
        /// \param[in] iFormat
       /// \param[in] iMetaData    Additional information
       /// \param[in] iIndex    If index != -1 and iFileName doesn't end with TIFF_FILE_EXT then this index is concatenate to iFileName
        /// \return InuCommon::CTiffFile::EError    eOK in success or error code.
        static EError Write(const std::string& iFileName,
                            const unsigned char* iBuffer,
                            unsigned int iWidth,
                            unsigned int iHeight,
                            CTiffFile::ETiffFormat iFormat,
                            const CMetaData& iMetaData = CMetaData(),
                            uint64_t iIndex = std::numeric_limits<uint64_t>::max(),
                            const std::string& iFileNameSuffix = std::string("")
        );

        /// \brief    Retrieve image data and attributes from input tiff file
        /// \param[out] oFileName    Input file name.
        /// \param[out] oBuffer    If oBuffer is nullptr then it will be allocated and initialized by this method.
        /// \param[out] oWidth
        /// \param[out] oHeight
        /// \param[out] oBytesPerPixel
        /// \param[out] oFormat
        /// \param[out] oMetaData
        /// \return InuCommon::CTiffFile::EError    eOK in success or error code.
        static EError Read(const std::string& iFileName,
                           unsigned char*& oBuffer,
                           unsigned int& oWidth,
                           unsigned int& oHeight,
                           unsigned int& oBytesPerPixel,
                           CTiffFile::ETiffFormat& oFormat,
                           CMetaData& oMetaData);


        /// \brief    Retrieve image attributes from input tiff header
        /// \param[out] iFileName    Input file name.
        /// \param[out] oWidth
        /// \param[out] oHeight
        /// \param[out] oBytesPerPixel
        /// \param[out] oFormat
        /// \param[out] oMetaData
        /// \return InuCommon::CTiffFile::EError    eOK in success or error code.
        static EError ReadHeader(const std::string& iFileName,
                                 unsigned int& oWidth,
                                 unsigned int& oHeight,
                                 unsigned int& oBytesPerPixel,
                                 CTiffFile::ETiffFormat& oFormat,
                                 CMetaData& oMetaData);


        /// \brief   Retrieve image data from input tiff file.
        /// \param[out] iFileName    Input file name..
        /// \param[out] oBuffer    If oBuffer is nullptr then it will be allocated and initialized by this method..
        /// \param[out] iWidth    Image width must be provided to this method.
        /// \param[out] iHeight   Image height must be provided to this method..
        /// \param[out] iBytesPerPixel    Number of bytes per pixel must be provide.
        /// \return InuCommon::CTiffFile::EError   eOK in success or error code.
        static EError ReadData(const std::string& iFileName,
                               unsigned char*& oBuffer,
                               unsigned int iWidth,
                               unsigned int iHeight,
                               unsigned int iBytesPerPixel);

        /// \brief    Number of bytes per pixels according to tiff format
        static unsigned int BytesPerPixels(CTiffFile::ETiffFormat inputFormat);

    private:

        // Current version
        static const int CURRENT_VERSION_NUMBER = 16;

        // String written tags
        static const std::string INUITIVE_VENDOR_NAME;

        static std::map<CTiffFile::ETiffFormat,  std::string> FORMAT_NAME;

        // Inuitive's proprietary tags
        static const int TIFFTAG_INU_BASE = 5000;
        static const int TIFFTAG_INU_SENSORID = TIFFTAG_INU_BASE + 1;
        static const int TIFFTAG_INU_SENSORNAME = TIFFTAG_INU_BASE + 2 ;
        static const int TIFFTAG_INU_REVISION = TIFFTAG_INU_BASE + 3;
        static const int TIFFTAG_INU_RESOLUTION = TIFFTAG_INU_BASE + 4;
        static const int TIFFTAG_INU_INPUTTYPE = TIFFTAG_INU_BASE + 5;
        static const int TIFFTAG_INU_FORMAT = TIFFTAG_INU_BASE + 6;
        static const int TIFFTAG_INU_ADDIONAL_DATA = TIFFTAG_INU_BASE + 7;
        static const int TIFFTAG_INU_SENSOR_MODEL = TIFFTAG_INU_BASE + 8;   // Model name
        static const int TIFFTAG_INU_HW_TYPE = TIFFTAG_INU_BASE + 9;    // Type A0/B0
        static const int TIFFTAG_INU_DISPARITY_OFFSET = TIFFTAG_INU_BASE + 10;    // Offset per frame (only in B0)
        static const int TIFFTAG_INU_FRAME_INDEX = TIFFTAG_INU_BASE + 11;    // Frame index (as is set by FW)
        static const int TIFFTAG_INU_TIMESTAMP_HIGH = TIFFTAG_INU_BASE + 12;      // Frame time stamp  (as is set by FW)
        static const int TIFFTAG_INU_TIMESTAMP_LOW = TIFFTAG_INU_BASE + 13;      // Frame time stamp  (as is set by FW)
        static const int TIFFTAG_INU_MINDISPARITY = TIFFTAG_INU_BASE + 14;      // Minimum disparity  (as is set by FW)
        static const int TIFFTAG_INU_MAXDISPARITY = TIFFTAG_INU_BASE + 15;      // Maximum disparity  (as is set by FW)
        static const int TIFFTAG_INU_ANALOG_GAIN_LEFT = TIFFTAG_INU_BASE + 16;
        static const int TIFFTAG_INU_ANALOG_GAIN_RIGHT = TIFFTAG_INU_BASE + 17;
        static const int TIFFTAG_INU_DIGITAL_GAIN_LEFT = TIFFTAG_INU_BASE + 18;
        static const int TIFFTAG_INU_DIGITAL_GAIN_RIGHT = TIFFTAG_INU_BASE + 19;
        static const int TIFFTAG_INU_EXPOSURE_LEFT = TIFFTAG_INU_BASE + 20;
        static const int TIFFTAG_INU_EXPOSURE_RIGHT = TIFFTAG_INU_BASE + 21;
        static const int TIFFTAG_INU_TIFF_VERSION_NUMBER = TIFFTAG_INU_BASE + 22;
        static const int TIFFTAG_INU_TIFF_OPTICAL_FOCAL_LEN_LEFTX = TIFFTAG_INU_BASE + 23;
        static const int TIFFTAG_INU_TIFF_OPTICAL_FOCAL_LEN_LEFTY = TIFFTAG_INU_BASE + 24;
        static const int TIFFTAG_INU_TIFF_OPTICAL_FOCAL_LEN_RIGHTX = TIFFTAG_INU_BASE + 25;
        static const int TIFFTAG_INU_TIFF_OPTICAL_FOCAL_LEN_RIGHTY = TIFFTAG_INU_BASE + 26;
        static const int TIFFTAG_INU_TIFF_OPTICAL_CENTER_LEFTX = TIFFTAG_INU_BASE + 27;
        static const int TIFFTAG_INU_TIFF_OPTICAL_CENTER_LEFTY = TIFFTAG_INU_BASE + 28;
        static const int TIFFTAG_INU_TIFF_OPTICAL_CENTER_RIGHTX = TIFFTAG_INU_BASE + 29;
        static const int TIFFTAG_INU_TIFF_OPTICAL_CENTER_RIGHTY = TIFFTAG_INU_BASE + 30;
        static const int TIFFTAG_INU_TIFF_OPTICAL_OFFSET = TIFFTAG_INU_BASE + 31;
        static const int TIFFTAG_INU_TIFF_OPTICAL_BASELINE = TIFFTAG_INU_BASE + 32;
        static const int TIFFTAG_INU_TIFF_CALIBRATION_TEMPERATURE = TIFFTAG_INU_BASE + 33;
        static const int TIFFTAG_INU_TIFF_DEPTH_SCALE = TIFFTAG_INU_BASE + 34;
        static const int TIFFTAG_INU_TIFF_DEPTH_POS = TIFFTAG_INU_BASE + 35;
        static const int TIFFTAG_INU_TIFF_SENSOR_NUMBER_LEFT = TIFFTAG_INU_BASE + 36;
        static const int TIFFTAG_INU_TIFF_SENSOR_NUMBER_RIGHT = TIFFTAG_INU_BASE + 37;
        static const int TIFFTAG_INU_TIFF_SENSOR_FIRST = TIFFTAG_INU_BASE + 38;
        static const int TIFFTAG_INU_TIFF_OPTICAL_DISTORTION_LEFT = TIFFTAG_INU_BASE + 39;
        static const int TIFFTAG_INU_TIFF_OPTICAL_DISTORTION_RIGHT = TIFFTAG_INU_BASE + 40;
        static const int TIFFTAG_INU_TIFF_OPTICAL_DISTORTION_COUNT = TIFFTAG_INU_BASE + 41;
        static const int TIFFTAG_INU_TIFF_PROJECTOR_TYPE = TIFFTAG_INU_BASE + 42;
        static const int TIFFTAG_INU_TIFF_DEPTH_REGISTRATION_TYPE = TIFFTAG_INU_BASE + 43;
        static const int TIFFTAG_INU_ISP_GAIN_LEFT = TIFFTAG_INU_BASE + 44;
        static const int TIFFTAG_INU_ISP_GAIN_RIGHT = TIFFTAG_INU_BASE + 45;

        /// \brief    Indication if LibTiff has already been initialized
        static bool sIsInitialized;

        /// \brief    Must be called once before the first call for LibTiff
        static void Initialize();

        /// \brief    Used to extend LibTiff predefined tags.
        /// \param[in,out] tif    Extended tiff object.
        static void TagExtender(TIFF* tif);

        /// \brief    Check if input file is an old RAW format file (for backward compatibility).
        /// \param[in] iFileName    Checked file.
        /// \return bool    true if it is a deprecated file.
        static bool IsDeprecatedRawFile(const std::string& iFileName);

        /// \brief    Retrieve image data and attributes from input tiff file
        /// \param[out] oFileName    Input file name.
        /// \param[out] oBuffer    If oBuffer is nullptr then it will be allocated and initialized by this method.
        /// \param[out] oWidth
        /// \param[out] oHeight
        /// \param[out] oBytesPerPixel
        /// \param[out] oFormat
        /// \param[out] oMetaData
        /// \return InuCommon::CTiffFile::EError    eOK in success or error code.
//         static EError Read(const std::string& iFileName,
//             unsigned char*& oBuffer,
//             unsigned int& oWidth,
//             unsigned int& oHeight,
//             unsigned int& oBytesPerPixel,
//             ETiffFormat& oFormat,
//             CMetaData& oMetaData);


        /// \brief    Read header from Tiff object (other parameters are identical to the public function)
//         static EError ReadHeader(TIFF* tiff,
//                                  unsigned int& oWidth,
//                                  unsigned int& oHeight,
//                                  unsigned int& oBytesPerPixel,
//                                  ETiffFormat& oFormat,
//                                  CMetaData& oMetaData);
//
//
//         /// \brief    Read data from Tiff object (other parameters are identical to the public function)
//         static EError ReadData(TIFF* tiff,
//                                unsigned char*& oBuffer,
//                                unsigned int iWidth,
//                                unsigned int iHeight,
//                                unsigned int iBytesPerPixel);
    };

}

#ifdef _MSC_VER
#pragma warning(default : 4251)
#endif

#endif
