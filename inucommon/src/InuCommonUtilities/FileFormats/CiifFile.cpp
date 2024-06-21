#include "CiifFile.h"
#include "OSUtilities.h"

#include <sstream>
#include <fstream>
#include <list>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/regex.hpp> 

#ifdef _MSC_VER
#pragma warning( disable : 4996 ) //using sprintf because sprintf_s is not compiled on UBUNTU
#if (_MSC_VER < 1700)
//Visual studio version before 2012
#pragma warning(disable:4482)
#endif
#endif

static const int BITS_PER_NIBBLE = 4;

using namespace InuCommon;
using namespace std;


std::map<std::string, CCiifFile::ECiifFormat> CCiifFile::mPixelFormatMapper = boost::assign::map_list_of<std::string, ECiifFormat>
    ("(8)"  , RAW8 ) //  8 bit input data
    ("(12)" , RAW12) //  12 bit input data
    ("(16)" , RAW16) // 16 bit input data - regarded as 24 bit
    ("(24)" , RGB24) // 24 bit input data
    ("(32)" , RAW32) // 24 bit input data
    ("RGB24", RGB24) // 24 bit input data    
    ("YUV24", YUV24)
    ("RAW8" , RAW8 )
    ("RAW12", RAW12)
    ("RAW16", RAW16)
    ("RAW24", RAW24)
    ("RAW32", RAW32)
    ("RAW64", RAW64);

CCiifFile::CMetaData::CMetaData(
          int iWidth,
          int iHeight,
          int iNumberDisparities,
          int iNumberOfFrames,
          ECiifFormat iPixelFormat,
          ELineDirection iLineDirection,
          EDataFormat iDataFormat,
          const char iExportFormat[MAX_FORMAT_SIZE],
          unsigned int iBytesPerPixel):
    mWidth(iWidth),
    mHeight(iHeight),
    mNumberDisparities(iNumberDisparities),
    mNumberOfFrames(iNumberOfFrames),
    mPixelFormat(iPixelFormat),
    mDataFormat(iDataFormat),
    mBytesPerPixel(iBytesPerPixel),
    mLineDirection(iLineDirection)
{
    COsUtilities::strcpy(&mExportFormat[0], MAX_FORMAT_SIZE, iExportFormat);
}                      

const int CCiifFile::mBytesPerPixel[NUM_OF_PIXEL_FORMATS] =   {3, 3, 1, 2, 2, 3, 4, 8};

const int CCiifFile::mNibblesPerPixel[NUM_OF_PIXEL_FORMATS] = {6, 6, 2, 3, 4, 6, 8, 16};

const std::string CCiifFile::mHexOutWidth[] = {"%x","%01x","%02x","%03x","%04x","%05x","%06x","%07x","%08x","%09x","%10x","%11x","%12x","%13x","%14x","%15x","%16x"};

std::map<CCiifFile::ELineDirection, std::string> CCiifFile::mLineDirectionString = boost::assign::map_list_of<ELineDirection, std::string>
    (eLeftToRight, "LeftToRight");

CCiifFile::EError CCiifFile::Write(const std::string& iFileName,
                                   const unsigned char* iDataList,
                                   CMetaData& iMetaData,
                                   PixelSerializer iPixelSerializer,
                                   bool iWriteHeader)
{
    // Create directory
    boost::filesystem::path filePath(boost::filesystem::absolute(boost::filesystem::path(iFileName)));

    if (is_directory(filePath))
    {
        return eInvalidFileName;
    }

    boost::filesystem::path parentDirectory = filePath.parent_path();
    boost::filesystem::create_directory(parentDirectory); 

    // Open stream save file
    ofstream fileName(iFileName);    

    // Iterate through all the files to be saved
    try
    {
        if (iPixelSerializer == nullptr)
        {
            ExportCIIF(
                iFileName,
                iMetaData,
                0,
                iDataList,
                iWriteHeader);
        }
        else
        {
            ExportCIIF(
                iFileName,
                iMetaData,
                0,
                iDataList,
                iWriteHeader,
                iPixelSerializer);
        }
        
    }
    catch (...)
    {
        fileName.close();
        return eFailedWritingData;
    }        
    
    // Close saved file
    fileName.close();

    return CCiifFile::EError::eOK;
}

void CCiifFile::ExportCIIF(
    const std::string &iFileName,
    const CMetaData& iMetaData,
    int iFrameCounter,
    const unsigned char* iDataPointer,
    bool iWriteHeader,
    PixelSerializer iPixelSerializer)
{
    std::ofstream outputFile;
    outputFile.open(iFileName.c_str(), std::ios::app | std::ios::out);

    if (outputFile.good())
    {
        std::string ciifstr = " %0*x";

        if (iWriteHeader)
        {
            WriteHeader(outputFile, iMetaData);
        }

        char linebuffer[MAX_BUFFER_SIZE];
        int lineidx = 0;

        for (int y = 0; y < iMetaData.mHeight; ++y)
        {
            int line_offset = y*iMetaData.mWidth;

            for (int x = 0; x < iMetaData.mWidth; ++x)
            {
                int pixelHeaderWidth; // This is the pixel index data width (frameId, pixelLine, pixelRow)

                if (x <= 9999)
                {
                    sprintf(linebuffer + lineidx, "(%4d,%4d,%4d)", iFrameCounter, y, x);
                    pixelHeaderWidth = 16;
                }
                else
                {
                    sprintf(linebuffer + lineidx, "(%4d,%4d,%d)", iFrameCounter, y, x);
                    pixelHeaderWidth = 12 + int(log10(float(x)) + 1);
                }			
                
                lineidx += pixelHeaderWidth;

                // Add space between header and pixel value
                linebuffer[lineidx++] = ' ';

                // Write pixel data
                int dataOffset = (x + line_offset)*iMetaData.mBytesPerPixel;
                lineidx += iPixelSerializer(iDataPointer + dataOffset, linebuffer + lineidx);
            
                // Go to new line
                linebuffer[lineidx++] = '\n';

                // flush data to file - if buffer is full (no space for one more data line)
                if (lineidx + MAX_LINE_LENGTH >= MAX_BUFFER_SIZE)
                {
                    linebuffer[lineidx] = '\0';
                    outputFile << linebuffer;
                    lineidx = 0;
                }
            }
        }

        // At the end flush data to file
        linebuffer[lineidx] = '\0';
        outputFile << linebuffer;

        outputFile.close();
    }
}

void CCiifFile::ExportCIIF(
    const std::string &iFileName,
    const CMetaData& iMetaData,
    int iFrameCounter,
    const unsigned char* iDataPointer,
    bool iWriteHeader)
{  
    int sx_reduced = iMetaData.mWidth/iMetaData.mNumberDisparities;
    int bpp_out = mBytesPerPixel[iMetaData.mPixelFormat];
    std::ofstream outputFile;
    
    int nibbles_out = mNibblesPerPixel[iMetaData.mPixelFormat];    

    outputFile.open (iFileName.c_str(), std::ios::app | std::ios::out);

    if (outputFile.good())
    {
        int datwid[4] = {0, 0, 0, 0};
        int datshift[4] = {0, 0, 0, 0};
        uint64_t datand[4] = {0, 0, 0, 0};
        int iciiformat[4] = {0,0,0,0};
        int nibblesOut[4] = { nibbles_out * BITS_PER_NIBBLE, 0, 0, 0 };

        std::string ciifstr = " %0*x";        

        int ciif_desc_size = BuildCiifFormatDescriptor(nibblesOut, datwid, datshift, datand, iciiformat, ciifstr);

        if (iWriteHeader)
        {
            WriteHeader(outputFile, iMetaData, datwid, datshift, datand, iciiformat, ciifstr, ciif_desc_size);        
        }        
        
        char linebuffer[MAX_BUFFER_SIZE];
        int lineidx = 0;

        for (int y = 0; y < iMetaData.mHeight; ++y)
        {
            int line_offset = y*sx_reduced;           

            for (int x = 0; x < sx_reduced; ++x)
            {
                if(x <= 9999)                    
                    sprintf(linebuffer + lineidx, "(%4d,%4d,%4d)", iFrameCounter, y, x);
                else
                    sprintf (linebuffer + lineidx, "(%4d,%4d,%d)", iFrameCounter, y, x);

                int w=4;

                if(x>9999)
                {
                    w=int(log10(float(x))+1);
                }

                lineidx += (12+w);	// This is the pixel index data width (frameId, pixelLine, pixelRow)				

                for (int ndisp = 0; ndisp < iMetaData.mNumberDisparities; ++ndisp)
                {
                    int i = ndisp + (x + line_offset)*iMetaData.mNumberDisparities;
                    uint64_t out_hex = 0;
                    for (int n = 0; n < bpp_out; n++)
                    {
                        uint64_t tmp = uint64_t(iDataPointer[i*bpp_out + n]);
                        out_hex |= (tmp << 8*n);
                    }

                    if (datwid[1] == 0)
                    {
                        // Patch to fix a bug in the weird mechanism of "datand"
                        uint64_t mask;
                        if (iciiformat[0] == 64)
                            mask = 0xffffffffffffffff;
                        else
                            mask = (1ull << iciiformat[0]) - 1;

                        // Only 1 element is printed - Perform optimized conversion (this is the common code path)
                        linebuffer[lineidx++] = ' ';
                        PrintHexPadded(linebuffer + lineidx, out_hex >> datshift[0] & mask, datwid[0]);
                        lineidx += datwid[0];
                    }
                    else
                    {                        
                        uint64_t datel[4];
                        // More than 1 element is printed - do it the convenient way
                        datel[0] = (out_hex >> datshift[0]) & (datand[0]);
                        datel[1] = (out_hex >> datshift[1]) & (datand[1]);
                        datel[2] = (out_hex >> datshift[2]) & (datand[2]);
                        datel[3] = (out_hex >> datshift[3]) & (datand[3]);
                        sprintf(linebuffer + lineidx, ciifstr.c_str(),datwid[0],datel[0],datwid[1],datel[1],datwid[2],datel[2],datwid[3],datel[3]);
                        lineidx += (datwid[0] + datwid[1] + datwid[2] + datwid[3] + 1);
                        // Small patch that fixes a bug in an unknown place
                        while (linebuffer[lineidx] != '\0')
                            ++lineidx;
                    }
                }
                linebuffer[lineidx++] = '\n';

                // Check if there is space in buffer for one more data line
                // if not, flush data to file
                if (lineidx + MAX_LINE_LENGTH >= MAX_BUFFER_SIZE)
                {
                    linebuffer[lineidx] = '\0';
                    outputFile << linebuffer;    
                    lineidx = 0;
                }
            }               
        }

        // At the end flush data to file
        linebuffer[lineidx] = '\0';
        outputFile << linebuffer; 

        outputFile.close();
    }			
}

CCiifFile::EError CCiifFile::Read(
    const std::string& iFileName,
    CMetaData& oMetaData,	
    unsigned char*& oBuffer,
    PixelDeserializer iPixelDeserializer)
{
    if (iPixelDeserializer == nullptr)
    {
        iPixelDeserializer = StringToHex;
    }

    std::ifstream file(iFileName);

    CCiifFile::EError ret = ReadHeader(file, oMetaData);

    if (ret == eOK)
    {
        oMetaData.mBytesPerPixel =  mBytesPerPixel[oMetaData.mPixelFormat];
        
        ret = ReadData(file, oBuffer, oMetaData.mWidth, oMetaData.mHeight, oMetaData.mBytesPerPixel, oMetaData.mNumberOfFrames, iPixelDeserializer);
    }

    return ret;
}

CCiifFile::EError CCiifFile::ReadData(std::ifstream &ifs,
                                      unsigned char*& oBuffer,
                                      unsigned int iWidth,
                                      unsigned int iHeight,
                                      unsigned int iBytesPerPixel,
                                      unsigned int iNumberOfFrames,
                                      PixelDeserializer iPixelDeserializer)
{
    if (!ifs.good())
    {
        return eFileOpen;
    }

    unsigned int size = iWidth*iHeight*iBytesPerPixel*iNumberOfFrames;
    if (oBuffer == nullptr)
    {
        //size=(unsigned int)(boost::filesystem::file_size(iFileName));
        oBuffer = new unsigned char[size]; 
    }

    boost::regex expression("\\(\\s*+([0-9]+),\\s*+([0-9]+),\\s*+([0-9]+)\\)\\s*+([0-9|a-z|A-Z]+)"); 

    std::string line;
    unsigned int lineOffset = 0;

    while(std::getline(ifs, line))
    {
        if (line.empty())
        {
            continue;
        }

        boost::cmatch what; 

        std::string pixelValue;
        if(boost::regex_match(line.c_str(), what, expression)) 
        { 
            pixelValue = (what[4].first);
        }
                
        int numOfReadBytes = iPixelDeserializer(pixelValue, oBuffer + lineOffset);

        if ((unsigned int)numOfReadBytes != iBytesPerPixel)
        {
            return eUnexpectedFormat;
        }

        // Padding bytes in zeros
        for (unsigned int i=numOfReadBytes; i<iBytesPerPixel; ++i)
        {
            oBuffer[lineOffset+i] = 0;
        }

        lineOffset += iBytesPerPixel;
    }    

    return CCiifFile::eOK;
}

int CCiifFile::StringToHex(const std::string &iHexStr, unsigned char* oBuffer)
{    
    int byteIndex = 0;
    int numCharsToByte = 2;
    unsigned int stringIterator = 0;
    unsigned int numOfBytes = (unsigned int)ceil((float)iHexStr.length() / numCharsToByte);    
    unsigned int c;
    string HexPadded(iHexStr);
    
    // Pad with zero if odd number of nibbles
    int residue = (numOfBytes * numCharsToByte) - (int)iHexStr.length();
    if (residue != 0)
    {
        HexPadded.insert(0, residue, '0');
    }
    while (stringIterator < HexPadded.length())
    {
        std::string subString = HexPadded.substr(stringIterator, numCharsToByte);

        std::istringstream hex_chars_stream(subString);

        hex_chars_stream >> std::hex >> c;

        // First write the LSB substring 
        // and later write the MSB substring in the buffer 
        oBuffer[numOfBytes - byteIndex - 1] = c;

         ++byteIndex;
         stringIterator = byteIndex*numCharsToByte;
    }

    return byteIndex;
}

CCiifFile::EError CCiifFile::ReadHeader(std::ifstream &iFile,
                                        CMetaData& oMetaData)
{        
    std::string line;    
    
    std::getline(iFile, line);
    oMetaData.mWidth = atoi(line.substr(line.find_last_of("=")+1).c_str());

    std::getline(iFile, line);
    oMetaData.mHeight = atoi(line.substr(line.find_last_of("=")+1).c_str());

    std::getline(iFile, line);
    oMetaData.mNumberDisparities = atoi(line.substr(line.find_last_of("=")+1).c_str());

    std::getline(iFile, line);
    oMetaData.mNumberOfFrames = atoi(line.substr(line.find_last_of("=")+1).c_str());

    // parse pixel format
    std::getline(iFile, line);
    std::string pixelFormat = line.substr(line.find_last_of("=")+1);

    // convert line endings
    size_t pos = pixelFormat.find("\r");
    if (pos != std::string::npos)
    {
        pixelFormat = pixelFormat.erase(pos, 1);
    }

    if (mPixelFormatMapper.find(pixelFormat) == mPixelFormatMapper.end()) 
    {
        // pixel format not valid
        return eUnexpectedFormat;        
    }    

    oMetaData.mPixelFormat = mPixelFormatMapper.find(pixelFormat)->second;

    std::getline(iFile, line);
    oMetaData.SetLineDirection(line.substr(line.find_last_of("=")+1).c_str());

    std::getline(iFile, line);
    oMetaData.mDataFormat = eHex;
     
    return CCiifFile::eOK;
}

void CCiifFile::WriteHeader(
    std::ofstream &oOutputFile,
    const CMetaData& iMetaData)
{
    if (oOutputFile.good())
    {	
        oOutputFile << "//Frame_width=" << iMetaData.mWidth << endl;
        oOutputFile << "//Frame_height=" << iMetaData.mHeight << endl;
        oOutputFile << "//Number_disparities=" << iMetaData.mNumberDisparities << endl;
        oOutputFile << "//Number_frames=" << 1 << endl;
        oOutputFile << "//Pixel_format=USER_SPECIFIC" << endl;
        oOutputFile << "//Line_direction=" << iMetaData.GetLineDirection().c_str() << endl;
        oOutputFile << "//Data_format=" << iMetaData.mExportFormat << endl;
    }
}

void CCiifFile::WriteHeader(
    std::ofstream &oOutputFile,
    const CMetaData& iMetaData,
    int (&datwid)[4], 
    int (&datshift)[4], 
    uint64_t (&datand)[4], 
    int (&iciiformat)[4],
    std::string iCiifstr,
    int ciif_desc_size)
{  
    int sx_reduced = iMetaData.mWidth/iMetaData.mNumberDisparities;
    //const char* hex_exp_format_out = mHexOutWidth[nibbles_out].c_str();

    if (oOutputFile.good())
    {
        if (sx_reduced == 0 || iMetaData.mHeight == 0)
        {
            oOutputFile.close();
        }

        //header (example): --- each header line should start with '//' ---
        oOutputFile << "//Frame_width=" << sx_reduced << endl;
        oOutputFile << "//Frame_height=" << iMetaData.mHeight << endl;
        oOutputFile << "//Number_disparities=" << iMetaData.mNumberDisparities << endl;
        oOutputFile << "//Number_frames=" << 1 << endl;

        //Pixel_format=(4,1,1,8)
        
        oOutputFile << "//Pixel_format=(";

        for (int i = 0; i <= ciif_desc_size; ++i)
        {
            oOutputFile << iciiformat[i];
            if (i < ciif_desc_size)
            {
                oOutputFile << ",";
            }
        }

        oOutputFile << ")" << endl;
        
        oOutputFile << "//Line_direction=" << iMetaData.GetLineDirection().c_str() << endl;
        oOutputFile << "//Data_format=" << iMetaData.mExportFormat << endl;
    }
}

int CCiifFile::BuildCiifFormatDescriptor(
    const int *p_ciiformat, 
    int (&_datwid)[4], 
    int (&_datshift)[4], 
    uint64_t (&_datand)[4], 
    int (&_iciiformat)[4], 
    std::string &_ciifstr)
{
    int ciif_desc_size = 0;

    int lastoutput = 0;
    if (p_ciiformat[1] != 0)
        lastoutput = 1;
    if (p_ciiformat[2] != 0)
        lastoutput = 2;
    if (p_ciiformat[3] != 0)
        lastoutput = 3;

    if (lastoutput == 0)
    {
        _iciiformat[0] = p_ciiformat[0];
    }
    else if (lastoutput == 1)
    {
        _iciiformat[0] = p_ciiformat[1];
        _iciiformat[1] = p_ciiformat[0];
    }
    else if (lastoutput == 2)
    {
        _iciiformat[0] = p_ciiformat[2];
        _iciiformat[1] = p_ciiformat[1];
        _iciiformat[2] = p_ciiformat[0];
    }
    else if (lastoutput == 3)
    {
        _iciiformat[0] = p_ciiformat[3];
        _iciiformat[1] = p_ciiformat[2];
        _iciiformat[2] = p_ciiformat[1];
        _iciiformat[3] = p_ciiformat[0];
    }
    if (lastoutput == 0)
    {
        _datwid[0] = (p_ciiformat[0]+3)>>2;
        _datand[0] = (uint64_t)((uint64_t)1<<p_ciiformat[0])-1;
    }
    if (lastoutput == 1)
    {
        _ciifstr += "_%0*x";
        _datwid[0] = (p_ciiformat[1]+3)>>2;
        _datwid[1] = (p_ciiformat[0]+3)>>2;
        _datshift[0] = p_ciiformat[0];
        _datand[0] = ((uint64_t)1<<p_ciiformat[1])-1;
        _datand[1] = ((uint64_t)1<<p_ciiformat[0])-1;
        ciif_desc_size = 1;
    }
    if (lastoutput == 2)
    {
        _ciifstr += "_%0*x_%0*x";
        _datwid[0] = (p_ciiformat[2]+3)>>2;
        _datwid[1] = (p_ciiformat[1]+3)>>2;
        _datwid[2] = (p_ciiformat[0]+3)>>2;
        _datshift[0] = p_ciiformat[0]+p_ciiformat[1];
        _datshift[1] = p_ciiformat[0];
        _datand[0] = ((uint64_t)1<<p_ciiformat[2])-1;
        _datand[1] = ((uint64_t)1<<p_ciiformat[1])-1;
        _datand[2] = ((uint64_t)1<<p_ciiformat[0])-1;
        ciif_desc_size = 2;
    }
    if (lastoutput == 3)
    {
        _ciifstr += "_%0*x_%0*x_%0*x";
        _datwid[0] = (p_ciiformat[3]+3)>>2;
        _datwid[1] = (p_ciiformat[2]+3)>>2;
        _datwid[2] = (p_ciiformat[1]+3)>>2;
        _datwid[3] = (p_ciiformat[0]+3)>>2;
        _datshift[0] = p_ciiformat[0]+p_ciiformat[1]+p_ciiformat[2];
        _datshift[1] = p_ciiformat[0]+p_ciiformat[1];
        _datshift[2] = p_ciiformat[0];
        _datand[0] = ((uint64_t)1<<p_ciiformat[3])-1;
        _datand[1] = ((uint64_t)1<<p_ciiformat[2])-1;
        _datand[2] = ((uint64_t)1<<p_ciiformat[1])-1;
        _datand[3] = ((uint64_t)1<<p_ciiformat[0])-1;
        ciif_desc_size = 3;
    }

    return ciif_desc_size;
}

// Convert an int to hex string padded with leading zeros (optimized for common case)
void CCiifFile::PrintHexPadded(char * dest, uint64_t val, uint64_t length)
{
    const char *hexchars = "0123456789abcdef";
    char* rev_iter = dest + length-1;
    for (uint64_t i = 0; i < length; ++i, --rev_iter, val >>= 4)
        *rev_iter = hexchars[val & 0xf];
}
