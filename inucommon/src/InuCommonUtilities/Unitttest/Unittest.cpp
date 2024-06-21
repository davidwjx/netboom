#include "TiffFile.h"
#include "FileSystemStorage.h"
#include "OSUtilities.h"
#include "RecordingMetaData.h"
#include "ProcessWrapper.h"
#include "Logger.h"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>


#include "boost/filesystem.hpp"

using namespace std;
using namespace InuCommon;

void ReadTiff(const string iFileName, unsigned char* iBaselineBuffer=nullptr)
{
    unsigned int width, height, bytesPerPixel;
    CTiffFile::ETiffFormat format;
    CTiffFile::CMetaData metaData;
    unsigned char* buffer = nullptr;
    CTiffFile::EError result=CTiffFile::Read(iFileName,buffer, width, height, bytesPerPixel, format, metaData);
    if (result != CTiffFile::eOK)
    {
        cout << "\nFailed to read file: " << iFileName << " error: " << result << endl;
    }
    else
    {
        cout << "\nFile: " << iFileName << "\n-------";
        cout << "\nFormat: " << format << "\nWidth: " << width << "\nHeight: " << height << "\nBytes Per Pixel: " << bytesPerPixel << endl;
        cout << "\nID: " << metaData.SensorID.Get() << "\nName: " <<metaData.SensorName.Get() << "\nResolution: " 
             << metaData.SensorResolution.Get() << "\nRevision: " << metaData.CalibrationRevision.Get() 
             << "\nInterleaved: " << metaData.InputType.Get() << "\nAddtional Data: " << metaData.AddtionalData.Get() << endl
             << "\nHWType: " << metaData.HWType.Get() << "\nDisparity Offset: " << metaData.DisparityOffset.Get() << endl
             << "\nFrameIndex: " << metaData.FrameIndex.Get() << "\nTimestamp: " << metaData.Timestamp.Get() << endl
             << "\nMinDisparity: " << metaData.MinDisparity.Get() << "\nMaxDisparity: " << metaData.MaxDisparity.Get() << endl
            << "\nAnalogGain: " << metaData.AnaglogGain.Get().first << "  " << metaData.AnaglogGain.Get().second << endl
            << "\nISPGain: " << metaData.IspGain.Get().first << "  " << metaData.IspGain.Get().second << endl
            << "\nDigitalGain: " << metaData.DigitalGain.Get().first << "  " << metaData.DigitalGain.Get().second << endl
             << "\nExposure: " << metaData.ExposureTime.Get().first << "  " << metaData.ExposureTime.Get().second << endl;

        if (iBaselineBuffer != nullptr)
        {
            if (memcmp(iBaselineBuffer, buffer, width*height*bytesPerPixel))
            {
                cout << "\nBuffers are different" << endl;
            }
            else
            {
                cout << "\nBuffers are equal" << endl;
            }
        }
    }
}

void TestTiff(bool iRead)
{
    static const string RGB_FILE("TestRGB.tif");
    static const string RAW_FILE("TestRAW.tif");
    static const string DEPTH_FILE("TestDepth.tif");


    // RGB
    static const unsigned int width(100),height(100);
    unsigned char buffer[width*height*4];
    memset(buffer, 255, width*height*4);
    unsigned int i=0;
    for (; i<width*height ; i+=4)
    {
        buffer[i] = 0;
    }
    for (; i<width*height*2 ; i+=4)
    {
        buffer[i+1] = 0;
    }
    for (; i<width*height*3 ; i+=4)
    {
        buffer[i+2] = 0;
    }

    // Depth
    unsigned char bufferDepth[width*height*2];
    memset(bufferDepth, 255, width*height*2);
    for (unsigned int i=0; i<height ; i++)
    {
        for (unsigned int j=0; j<width*2 ; j+=2)
        {
            *((short*)(bufferDepth+i*width*2+j)) = (1 << (j/10));
        }
    }


    // ODP Raw
    unsigned char bufferRAW[width*height*2];
    memset(bufferRAW, 255, width*height*2);
    for (unsigned int i=0; i<height ; i++)
    {
        for (unsigned int j=0; j<width*2 ; j+=2)
        {
            *((short*)(bufferRAW+i*width*2+j)) = (1 << (j/10));
        }
    }

    if (iRead)
    {
        ReadTiff(RGB_FILE, buffer);
        ReadTiff(RAW_FILE, bufferRAW);
        ReadTiff(DEPTH_FILE, bufferDepth);
    }
    else
    {
        CTiffFile::CMetaData metaData;
        metaData.SensorID.Set("SensorID");
        metaData.SensorName.Set("SensorName");
        metaData.SensorResolution.Set(CTiffFile::eBinning);
        metaData.CalibrationRevision.Set(2);
        metaData.InputType.Set(CTiffFile::eInterleaved);
        metaData.AddtionalData.Set("Free text");
        metaData.HWType.Set(CTiffFile::eB0);
        metaData.FrameIndex.Set(19);
        metaData.Timestamp.Set((uint64_t(2) << 33) + 1);
        metaData.DisparityOffset.Set(8);
        metaData.MaxDisparity.Set(1000);
        metaData.MinDisparity.Set(2);
       // metaData.AnaglogGain.Set(make_pair(2,3));
       // metaData.DigitalGain.Set(make_pair(4,5));
       // metaData.ExposureTime.Set(make_pair(6,7));


        CTiffFile::EError result=CTiffFile::Write(RGB_FILE,buffer,width, height, CTiffFile::eRGBA, metaData);
        if (result != CTiffFile::eOK)
        {
            cout << "Failed to write file: " << RGB_FILE << " error: " << result << endl;
        }

        result=CTiffFile::Write(DEPTH_FILE, bufferDepth,width, height, CTiffFile::eDepth);
        if (result != CTiffFile::eOK)
        {
            cout << "Failed to write file: " << DEPTH_FILE << " error: " << result << endl;
        }

        result=CTiffFile::Write(RAW_FILE,bufferRAW,width, height, CTiffFile::eYUV);
        if (result != CTiffFile::eOK)
        {
            cout << "Failed to write file: " << RAW_FILE << " error: " << result << endl;
        }
    }
}

void SaveBMP()
{
    // RGB
    static const unsigned int width(100),height(100);
    unsigned char buffer[width*height*3];
    memset(buffer, 255, width*height*3);
    unsigned int i=0;
    for (; i<width*height ; i+=3)
    {
        buffer[i] = 0;
    }
    for (; i<width*height*2 ; i+=3)
    {
        buffer[i+1] = 0;
    }
    for (; i<width*height*3 ; i+=3)
    {
        buffer[i+2] = 0;
    }

  	cout << "Save BMP result: " << COsUtilities::SaveBMP(buffer, width, height, 8, width*height*3, "testSave.bmp") << endl;
}

void BenchMark()
{
    static const string RGB_FILE("Output/TestRGB");
    static const string RAW_FILE("Output/TestRAW");
    static const string DEPTH_FILE("Output/TestDepth");

    if (boost::filesystem::exists("Output") )
    {
        boost::filesystem::remove_all("Output");
    }
    boost::filesystem::create_directory("Output");

    COsUtilities::RecordingFileList  listVideo,listDepth;
    COsUtilities::ListAllFiles("d:/Data/Roy","tif",listVideo,"Video");
    COsUtilities::ListAllFiles("d:/Data/Roy","tif",listDepth,"Depth");

    time_t start,end;
    start = COsUtilities::GetTimeHighRes();

    unsigned char* buffer = nullptr;
    unsigned int width, height, bpp;
    CTiffFile::ETiffFormat  format;
    CTiffFile::CMetaData metaData;

    for (size_t k=0; k < listVideo.size() ;k++)
    {
        if (CTiffFile::Read(listVideo[k].mFilename, buffer, width, height, bpp, format, metaData) != CTiffFile::eOK)
            break;

        ostringstream  name;
        name << RGB_FILE << "_" << k << ".tif";
        if (CTiffFile::Write(name.str(), buffer, width, height, format, metaData) != CTiffFile::eOK)
            break;     
    }

    for (size_t k=0; k < listDepth.size() ;k++)
    {

        if (CTiffFile::Read(listDepth[k].mFilename, buffer, width, height, bpp, format, metaData) != CTiffFile::eOK)
            break;

        ostringstream  name;
        name << DEPTH_FILE << "_" << k << ".tif";
        if (CTiffFile::Write(name.str(), buffer, width, height, format, metaData) != CTiffFile::eOK)
            break;
    
    }

    end = COsUtilities::GetTimeHighRes();
    cout << "Time: " << (end-start) <<  endl; 
}

void MetaDataFile()
{
    ostringstream  fileName;
    fileName << "MetaData_" << InuCommon::COsUtilities::GetTimeHighRes();

    CRecordingMetaData   metaData;
    metaData.mSensorID = "SensorID";
    //metaData.mOpticalData.WebcamDataValid = true;
    
    uint64_t time = COsUtilities::GetTimeHighRes();

    if (!metaData.Save(InuCommon::IStorage::CreateStorage(InuCommon::IStorage::eXML, "", fileName.str())))
    {
        cout << "Failed to write meta data file: " << endl;
    }

}

int main (int argc, char** argv) 
{
    cout << "=====  CommonUtilities Test Program =====\n\n";
    int testType(0);

    bool inputDefined=false;
    if (argc > 1)
    {
        ifstream   inputFile(argv[1]);
 
        int testType = 0;
        do
        {
            string key, value;
            CFileSystemStorage::GetNextRecord(key, value, inputFile);
            if (!inputFile.good())
            {
                break;
            }
            if (value.empty())
            {
                continue;
            }

            istringstream tmpStream(value);
            tmpStream >> testType;
            switch (testType)
            {
            case 1:
                TestTiff(true);
                break;
            case 2:
                TestTiff(false);
                break;
            case 3:
                CFileSystemStorage::GetNextRecord(key, value, inputFile);
                ReadTiff(value);
                break;
            case 4:
                BenchMark();
                break;
            case 5:
                MetaDataFile();
                break;
            case 8:
                cout << COsUtilities::GetCommonConfigPath() << endl;
                break;
            case 9:
                CFileSystemStorage::GetNextRecord(key, value, inputFile);
                cout << COsUtilities::IsProcessRunning(value) << endl;
                break;
            case 10:
                CFileSystemStorage::GetNextRecord(key, value, inputFile);
                cout << COsUtilities::GetExeFullPathFileName(value) << endl;
                break;
            case 11:
                CFileSystemStorage::GetNextRecord(key, value, inputFile);
                cout << "Default Severity " << CLogger::GetSeverity() << endl;
                cout << "Unittest Severity " << CLogger::GetSeverity("Unittest") << endl;
                CLogger::SetLogType(CLogger::ELogType(stoul(value)));
                CLogger::SetSeverity("Unittest", CLogger::eWarning);
                cout << "Unittest Severity " << CLogger::GetSeverity("Unittest") << endl;
                CLogger::Write("Test Error", CLogger::eError, "Unittest");
                CLogger::Write("Test Warning", CLogger::eWarning, "Unittest");
                CLogger::Write("Test Info",CLogger::eInfo, "Unittest");
                CLogger::Flush();
                break;
            case 12:
                SaveBMP();
                break;
            case 13:
            {
                CLogger::SetLogType(CLogger::ELogType(7));
                CLogger::SetSeverity("Common", CLogger::eInfo);

                const std::string strExpectedOutput { "####################### Unit-testing CProcessWrapper ##########################" };

                InuCommon::CProcessWrapper process;

                //////////////////////////////////////////////////////////////////////////
                // Run using CreateProcess 
                //////////////////////////////////////////////////////////////////////////
                
                auto result = process.RunProcess(R"(C:\Windows\System32\cmd.exe)", "/C echo " + strExpectedOutput, true);
                CLogger::Flush();
                cout << "RunProcess result = " << (int)result << "\n";
                assert(result == InuCommon::CProcessWrapper::EResultCodes::eSUCCESS);

                cout << "exit code " << process.GetExitCode() << "\n";
                assert(process.GetExitCode() == 0);

                cout << "output\n";
                for (const auto & line : process.GetProcessOutput()) {
                    cout << line << "\n";
                }

                auto output1 = process.GetProcessOutput();
                assert(!output1.empty());

                //////////////////////////////////////////////////////////////////////////
                // Run using popen 
                //////////////////////////////////////////////////////////////////////////

                //result = process.RunCommand(R"(c:\Windows\System32\systeminfo.exe)", true, true);
                result = process.RunCommand("echo " + strExpectedOutput, true, true);
                cout << "RunCommand (script using popen) result = " << (int)result << "\n";
                assert(result == InuCommon::CProcessWrapper::EResultCodes::eSUCCESS);
                CLogger::Flush();

                result = process.RunCommand(R"(C:\Windows\System32\cmd.exe /C echo )" + strExpectedOutput, true, true);
                CLogger::Flush();
                cout << "RunCommand (script using cmp.exe and popen) result = " << (int)result << "\n";
                assert(result == InuCommon::CProcessWrapper::EResultCodes::eSUCCESS);

                cout << "exit code " << process.GetExitCode() << "\n";
                assert(process.GetExitCode() == 0);

                cout << "output\n";
                for (const auto & line : process.GetProcessOutput()) {
                    cout << line << "\n";
                }

                auto output2 = process.GetProcessOutput();
                assert(!output2.empty());

                //////////////////////////////////////////////////////////////////////////
                // Compare results
                //////////////////////////////////////////////////////////////////////////

                assert(output1.size() == output2.size());
                assert(output1 == output2);
                assert(output1[0] == strExpectedOutput);
            }

                break;

            default:
                break;
            }
        } while (testType != 0);
    }

    else
    {
        do 
        {
            cout << "Please insert required test (0 - Quit, 1 - ReadTiff, 2 - WriteTiff, 3 - Read Input tiff): ";
            cin >> testType;
            switch (testType)
            {
            case 1:
                TestTiff(true);
                break;
            case 2:
                TestTiff(false);
                break;
            case 3:
                {
                    string fileName;
                    cout << "Please insert input tiff file: ";
                    cin >> fileName;
                    ReadTiff(fileName);
                }
                break;
            case 4:
                BenchMark();
                break;
            case 5:
                MetaDataFile();
                break;
            default:
                break;
            }
        } while (testType != 0);
    }

}
