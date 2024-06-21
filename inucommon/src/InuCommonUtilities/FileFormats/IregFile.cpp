#include "OSUtilities.h"
#include "IregFile.h"

#include <iomanip>
#include <fstream>
#include <sstream>
#include <limits.h>

#include <boost/algorithm/string.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/lexical_cast.hpp>

#if (defined(_MSC_VER) && (_MSC_VER <1700)) //Visual studio version before 2012
#pragma warning(disable:4482)
#endif

using namespace InuCommon;
using namespace std;

static const string WRITE("W");
static const string READ("R");
static const string VIDEO("video");
static const string DEPTH("depth");
static const string WEBCAM("webcam");
static const string IMAGE_FORMAT("image");
static const string SHIFT_Y("shifty");
static const string METADATA_PREFIX("#*#");
static const string WAIT_COMMAND("wait");
static const string SENSER_REG_NAME("reg");
static const string SENSOR_SEN_NAME("sen");
static const string SENSOR_I2C_NAME("i2c");

static const int  DEFAULT_PHASE_OPTION = 1;

std::map<CRegistersSet::RegisterType, std::string> CIregFile::mRegisterTypeToDescription = boost::assign::map_list_of
    (CRegistersSet::RegisterType::eI2C, "I2C")
    (CRegistersSet::RegisterType::eRegister, "REG")
    (CRegistersSet::eSensor, "SEN")
    (CRegistersSet::eWait, "WAIT");

CIregFile::EError CIregFile::Write(const std::vector<CRegistersSet::CRegisterData>& iBuffer,
                                         const std::string& iFileName,
                                         std::pair<unsigned int, unsigned int>& videoMapSize,
                                         std::pair<unsigned int, unsigned int>& depthMapSize,
                                         unsigned int iImageFormat,
                                         unsigned int iShiftY,
                                         unsigned int iPhase)
{
    ofstream file(InuCommon::COsUtilities::ConvertEncoding(iFileName));
    if (!file.good())
    {
        return eFileOpen;
    }

    std::stringstream deviceAddress;
    std::stringstream address;
    std::stringstream data;
    std::stringstream mask;
    std::stringstream phase;

    // Write registers
    for(const auto& reg : iBuffer)
    {
        phase << std::hex << std::uppercase << std::setw(1) << reg.mPhase;

        switch (reg.mType)
        {
        case CRegistersSet::eI2C:

            deviceAddress << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << reg.mDeviceAddress;
            address << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << reg.mAddress;
            data << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << reg.mData;

            file << mRegisterTypeToDescription[reg.mType] << " "
                 << reg.mI2cNum << " "
                 << (reg.mWrite ? WRITE : READ)
                 << " 0x" << deviceAddress.str()
                 << " " << reg.mAddressSize
                 << " 0x" << address.str()
                 << " " << reg.mSize
                 << " 0x" << data.str()
                 << (reg.mPhase ? " " + phase.str() : "")
                 <<  std::endl;

            // Clean stringstream for next iteration
            deviceAddress.str(std::string());
            address.str(std::string());
            data.str(std::string());;

            break;

        case CRegistersSet::eRegister:

            address << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << reg.mAddress;
            data << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << reg.mData;
            mask << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << reg.mMask;

            file << mRegisterTypeToDescription[reg.mType]
                << " 0x" << address.str()
                << ((reg.mMask != UINT_MAX) ? " 0x" + mask.str() : "")
                << " = 0x" << data.str()
                << (reg.mPhase != 1 ? " " + phase.str() : "")
                << std::endl;

            // Clean stringstream for next iteration
            address.str(std::string());
            data.str(std::string());
            mask.str(std::string());

            break;

        case CRegistersSet::eWait:

            file << mRegisterTypeToDescription[reg.mType] << " " << reg.mData << std::endl;

            break;

        case CRegistersSet::eSensor:
        default:
            break;
        }

        phase.str(std::string());
    }
    
    file << std::endl ;

    // Write meta data
    file << "### Metadata - DO NOT CHANGE !" << std::endl;

    file << METADATA_PREFIX << " Video" << " Size " << videoMapSize.first << " " << videoMapSize.second << std::endl;

    file << METADATA_PREFIX << " Depth" << " Size " << depthMapSize.first << " " << depthMapSize.second << std::endl;

    file << METADATA_PREFIX << " Image Format " << iImageFormat << std::endl;
    
    file << METADATA_PREFIX << " ShiftY " << iShiftY << std::endl;

    file.close();

    return eOK;
}

CIregFile::EError CIregFile::Read(std::vector<CRegistersSet::CRegisterData>& oResult,
                                  const std::string& iFileName,
                                  std::pair<unsigned int, unsigned int>& videoMapSize,
                                  std::pair<unsigned int, unsigned int>& depthMapSize,
                                  std::pair<unsigned int, unsigned int>& webcamSize,
                                  unsigned int& oImageFormat,
                                  unsigned int& oShiftY,
                                  string& oConfigIregVer,
                                  bool iIsOdpReg,
                                  unsigned int iPhase)
{
    ifstream file(InuCommon::COsUtilities::ConvertEncoding(iFileName));
    if (!file.good())
    {
        return eFileOpen;
    }

    while (file.good() && !file.eof()) 
    {      
        string line;
        getline(file, line);
        istringstream lineStream(line);

        //separate the line to a vector of strings
        vector<std::string> stringVec;
        copy(istream_iterator<string>(lineStream), istream_iterator<string>(), back_inserter(stringVec));

        if (stringVec.size() == 0)
        {
            continue;
        }

        int tokenIndex = 0;
        string RegisterType = stringVec[tokenIndex++];
        if (RegisterType.size() == 0)
        {
            continue;
        }

        if (RegisterType[0] == '#')
        {
            if (RegisterType == METADATA_PREFIX)
            {
                //#*# Video Size 1232 916 
                //#*# Depth Size 616 458
                std::string firstWord = stringVec[tokenIndex++];

                boost::algorithm::to_lower(firstWord);

                if (firstWord == VIDEO)
                {
                    //skip the word "Size"
                    tokenIndex++;
                    //<width,height>
                    videoMapSize.first = std::stoul(stringVec[tokenIndex++], nullptr, 10);
                    videoMapSize.second = std::stoul(stringVec[tokenIndex++], nullptr, 10);
                }
                /*
                else if ((firstWord.find(VIDEO) != std::string::npos) && (std::any_of(firstWord.begin(), firstWord.end(), ::isdigit)))
                {
                    std::string rev = std::string(firstWord.begin() + std::string(VIDEO).length(),firstWord.end());
                    int channel = boost::lexical_cast<unsigned int>(rev);

                    //skip the word "Size"
                    tokenIndex++;

                    //<width,height>
                    videoMapSize[channel].first = std::stoul(stringVec[tokenIndex++], nullptr, 10);
                    videoMapSize[channel].second = std::stoul(stringVec[tokenIndex++], nullptr, 10);
                }
                */
                else if (firstWord == DEPTH)
                {
                    //skip the word "Size"
                    tokenIndex++;
                    //<width,height>
                    depthMapSize.first = std::stoul(stringVec[tokenIndex++],nullptr, 10);
                    depthMapSize.second = std::stoul(stringVec[tokenIndex++],nullptr, 10);
                }
                /*
                else if ((firstWord.find(DEPTH) != std::string::npos) && (std::any_of(firstWord.begin(), firstWord.end(), ::isdigit)))
                {
                    std::string rev = std::string(firstWord.begin() + std::string(DEPTH).length(),firstWord.end());
                    int channel = boost::lexical_cast<unsigned int>(rev);

                    //skip the word "Size"
                    tokenIndex++;

                    //<width,height>
                    depthMapSize[channel].first = std::stoul(stringVec[tokenIndex++], nullptr, 10);
                    depthMapSize[channel].second = std::stoul(stringVec[tokenIndex++], nullptr, 10);
                }
                */
                else if (firstWord == WEBCAM)
                {
                    //skip the word "Size"
                    tokenIndex++;
                    //<width,height>
                    webcamSize.first = std::stoul(stringVec[tokenIndex++],nullptr, 10);
                    webcamSize.second = std::stoul(stringVec[tokenIndex++],nullptr, 10);
                }
                /*
                else if ((firstWord.find(WEBCAM) != std::string::npos) && (std::any_of(firstWord.begin(), firstWord.end(), ::isdigit)))
                {
                    std::string rev = std::string(firstWord.begin() + std::string(WEBCAM).length(),firstWord.end());
                    int channel = boost::lexical_cast<unsigned int>(rev);

                    //skip the word "Size"
                    tokenIndex++;

                    //<width,height>
                    webcamSize[channel].first = std::stoul(stringVec[tokenIndex++], nullptr, 10);
                    webcamSize[channel].second = std::stoul(stringVec[tokenIndex++], nullptr, 10);
                }
                */
                else if (firstWord == IMAGE_FORMAT)
                {
                    //skip the word "Format"
                    tokenIndex++;
                    oImageFormat = std::stoul(stringVec[tokenIndex++],nullptr, 10);
                }
                else if (firstWord == SHIFT_Y)
                {
                    oShiftY = std::stoul(stringVec[tokenIndex++],nullptr, 10);
                }
                //Config.ireg Version
                else if (firstWord == "configurator")
                {
                    oConfigIregVer = stringVec[++tokenIndex];
                }

            }
            // TODO: handle comments
            continue;
        }

        CRegistersSet::CRegisterData reg;
        boost::algorithm::to_lower(RegisterType);

        if (RegisterType == SENSER_REG_NAME) 
        {
            reg.mType = CRegistersSet::eRegister;
            if (stringVec.size() >= 4) 
            {
                //should include a phase definition:
                //    REG 0x48001500 = 0x00020000 1  
                // or REG 0x48001500 0xF0FF0 = 0x00020000 1 
                reg.mAddress = std::stoul(stringVec[tokenIndex++], nullptr, 16);
                if (stringVec[tokenIndex] != "=")
                {
                    reg.mMask = std::stoul(stringVec[tokenIndex++], nullptr, 16);
                }
                else
                {
                    reg.mMask = UINT_MAX;
                }
                tokenIndex++;   // ignore "="  
                reg.mData = std::stoul(stringVec[tokenIndex++], nullptr, 16);
            }
        }

        else if (RegisterType == SENSOR_I2C_NAME) // I2C registers
        {
            reg.mType = CRegistersSet::eI2C;
            //I2c register syntax: I2C i2cNum i2cReadWrite tarAddress accessSize regAddress regWidth i2cData phase
            if (stringVec.size() >= 8)
            {
                reg.mI2cNum = std::stoul(stringVec[tokenIndex++], nullptr, 10);
                reg.mWrite = stringVec[tokenIndex++] == WRITE;
                reg.mDeviceAddress = std::stoul(stringVec[tokenIndex++], nullptr, 16);
                reg.mAddressSize = std::stoul(stringVec[tokenIndex++], nullptr, 10);
                reg.mAddress = std::stoul(stringVec[tokenIndex++], nullptr, 16);
                reg.mSize = std::stoul(stringVec[tokenIndex++], nullptr, 10);
                reg.mData = std::stoul(stringVec[tokenIndex++], nullptr, 16);
            }
        }

        else if (RegisterType == WAIT_COMMAND) // Wait registers
        {
            reg.mType = CRegistersSet::eWait;
            //WAIT register syntax: WAIT usecWait phase
            if (stringVec.size() > size_t(tokenIndex))
            {
                reg.mData = std::stoul(stringVec[tokenIndex++], nullptr, 10);
            }
        }

        else 
        {
            if (RegisterType == SENSOR_SEN_NAME)
            {
                reg.mType = CRegistersSet::eSensor;
                reg.mLeftEye = stringVec[tokenIndex++] == "L";
                reg.mSize = stoul(stringVec[tokenIndex++], nullptr, 10);
            }

            // reg || sen
            reg.mAddress = stoul(stringVec[tokenIndex++], nullptr, 16);
            tokenIndex++; // ignore = 
            reg.mData = stoul(stringVec[tokenIndex++], nullptr, 16);
        }

        // Phase is common to all options, default is 0:
        reg.mPhase = DEFAULT_PHASE_OPTION;
        if (iPhase != UINT_MAX)
        {
            reg.mPhase = iPhase;
        }
        else if (stringVec.size() > size_t(tokenIndex))
        {
            reg.mPhase = std::stoul(stringVec[tokenIndex++], nullptr, 16);
        }
        if (iIsOdpReg)
        {
            for (auto& elem : oResult)
            {
                if (elem == reg)
                {
                    elem.mData = reg.mData;
                }
            }
            continue;
        }
        oResult.push_back(reg);
    }

    return eOK;
}
