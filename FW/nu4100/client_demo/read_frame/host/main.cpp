#include "InuSensorExt.h"
#include "UserDefinedStream.h"
#include "GeneralFrame.h"
#include "ImageStream.h"

#include "inu_types.h"
#include "basic.h"
#include "../common/basic_demo.h"


#include <iostream>
#include <fstream>
#include <string>
#include <memory>

using namespace InuDev;
using namespace std;

std::shared_ptr<InuDev::CInuSensorExt>  inuSensor;
std::shared_ptr<InuDev::CUserDefinedStream> fdkStream;
std::shared_ptr<InuDev::CUserDefinedStream> rgbProcessedStream;
std::shared_ptr<InuDev::CImageStream> RGBimageStream;

//We don't have the image size because we get RAW data from FDK User Define streams
static const int HEIGHT = 960;
static const int WIDTH = 1280;

// This callback is invoked when a data frame from the FDK is ready 
void UserDefinedCallback(std::shared_ptr<InuDev::CUserDefinedStream> iStream, std::shared_ptr<const InuDev::CGeneralFrame> iFrame,  CInuError iErrorCode)
{
    if (iErrorCode != InuDev::eOK)
    {
        std::cout << "Error in receiving frame: " << std::hex << int(iErrorCode) << " " << std::string(iErrorCode) << std::endl;
        return;
    }

    if (iFrame->Valid == false)
    {
        std::cout << "Frame  " << iFrame->FrameIndex << " is invalid" << std::endl;
        return;
    }
    
    // The buffer is actually a BASIC_DEMO_gpHostMsgStructT object that was created by the FDK
    BASIC_DEMO_gpHostMsgStructT* tt = (BASIC_DEMO_gpHostMsgStructT*)iFrame->GetData();
    
    if (iErrorCode == eOK && iFrame->Valid)
    {
        // Print the received data
        cout << " User counter: " << tt->counter;
        cout << " GPP usec: " << tt->gpUsec;
        cout << endl;
    }
 }

void DisplayRGBAfterFDK(std::shared_ptr<InuDev::CUserDefinedStream>, std::shared_ptr<const InuDev::CGeneralFrame>  iFrame, InuDev::CInuError)
{

	unsigned char* dataR = (unsigned char*)iFrame->GetData();
	
	if (iFrame->Valid == false)
	{
		std::cout << "Frame  " << iFrame->FrameIndex << " is invalid" << std::endl;
		return;
	}

	// Use OpenCV to display the image 
	cv::Mat img(HEIGHT, WIDTH, CV_8UC3, (uchar*)iFrame->GetData());
	cv::cvtColor(img, img, cv::COLOR_BGR2RGB);

	std::ostringstream oss;
	oss << "RGB Image after FDK " ;
	std::string win_name = oss.str();

	cv::namedWindow(win_name, cv::WINDOW_AUTOSIZE);
	cv::imshow(win_name, img);
	cv::waitKey(1);

}

void DisplayRGBBeforeFDK(std::shared_ptr<InuDev::CImageStream>, std::shared_ptr<const InuDev::CImageFrame>  iFrame, InuDev::CInuError)
{

	unsigned char* dataR = (unsigned char*)iFrame->GetData();

	if (iFrame->Valid == false)
	{
		std::cout << "Frame  " << iFrame->FrameIndex << " is invalid" << std::endl;
		return;
	}

	cv::Mat img;
	static cv::Mat img1;


	img = cv::Mat(iFrame->Height(), iFrame->Width(), CV_8UC4, (uchar*)iFrame->GetData()); 

	std::ostringstream oss;
	oss << "Original RGB image " << iFrame->Height() << "x" << iFrame->Width();
	std::string win_name = oss.str();

	cv::namedWindow(win_name, cv::WINDOW_AUTOSIZE);
	cv::imshow(win_name, img);
	cv::waitKey(1);

}

// Execute the "basic" demo test 
void Execute()
{
    // The SW graph should be overwritten in this demo
    InuDev::CDeviceParams inuSensorParams;
    CHwInformation oHWInfo;
	BASIC_DEMO_hostGpMsgStructT hostGPmsg;

    static char current_directory[200];

#ifdef _MSC_VER
    GetCurrentDirectoryA(sizeof(current_directory), current_directory);
#else
    getcwd(current_directory, sizeof(current_directory));
#endif
    inuSensorParams.GraphXmlPath = build_file_name(2, current_directory, "sw_graph_boot60_FDK.xml");
	inuSensorParams.FPS = 20;

    // Creation of CInuSensor object
    inuSensor = InuDev::CInuSensorExt::Create();

    // Initiate the sensor - it must be call before any access to the sensor. Sensor will start working in low power.   
    InuDev::CInuError retCode = inuSensor->Init(oHWInfo, inuSensorParams);
    if (retCode != InuDev::eOK)
    {
        std::cout << "Failed to connect to Inuitive Sensor. Error: " << std::hex << int(retCode) << " - " << std::string(retCode) << std::endl;
        return;
    }

    std::cout << "Connected to Sensor" << std::endl;

	// Start sensor
	retCode = inuSensor->Start();
	if (retCode != InuDev::eOK)
	{
		std::cout << "Failed to start to Inuitive Sensor." << std::endl;
		return;
	}

	std::cout << "Sensor is started" << std::endl;
    
	RGBimageStream = inuSensor->CreateImageStream(9);
	if (RGBimageStream == nullptr)
	{
		std::cout << "Unexpected error, failed to get imageStream Stream" << std::endl;
		return;
	}

	retCode = RGBimageStream->Init();
	if (retCode != InuDev::eOK)
	{
		std::cout << "imageStream initiation error: " << std::hex << int(retCode) << " - " << std::string(retCode) << std::endl;
		return;
	}
	std::cout << "imageStream Stream is initialized" << std::endl;

	retCode = RGBimageStream->Start();
	if (retCode != InuDev::eOK)
	{
		std::cout << "Start error: " << std::hex << int(retCode) << " - " << std::string(retCode) << std::endl;
		return;
	}
	std::cout << "imageStream frames acquisition started" << std::endl;

	retCode = RGBimageStream->Register(DisplayRGBBeforeFDK);
	if (retCode != InuDev::eOK)
	{
		std::cout << "WebCam register error: " << std::hex << int(retCode) << " - " << std::string(retCode) << std::endl;
		return;
	}

	fdkStream = inuSensor->CreateUserDefinedStream("Stream_out_FDK_RAW");

	//init & start FDK stream
	retCode = fdkStream->Init();

	if (retCode != InuDev::eOK)
	{
		std::cout << "FDK Stream initiation error: " << std::hex << int(retCode) << " - " << std::string(retCode) << std::endl;
		return;
	}
	std::cout << "FDK Stream is initialized" << std::endl;

	retCode = fdkStream->Start();
	if (retCode != eOK)
	{
		std::cout << "FDK stream failed to Start: " << string(retCode) << std::endl;
	}
	
	fdkStream->Register(UserDefinedCallback);

	memset(&hostGPmsg, 0, sizeof(BASIC_DEMO_hostGpMsgStructT));
	hostGPmsg.algInfo.enable = 1;
	hostGPmsg.algInfo.startX = 10;
	hostGPmsg.algInfo.startY = 10;
	hostGPmsg.algInfo.picWidth = 200;
	hostGPmsg.algInfo.picHeight = 200;

	retCode = inuSensor->ControlFDK((const unsigned char*)&hostGPmsg, sizeof(hostGPmsg), "FDK_1");

	rgbProcessedStream = inuSensor->CreateUserDefinedStream("Stream_out_FDK_image");

	retCode = rgbProcessedStream->Init();

	if (retCode != InuDev::eOK)
	{
		std::cout << "rgbProcessedStream Stream initiation error: " << std::hex << int(retCode) << " - " << std::string(retCode) << std::endl;
		return;
	}
	
	retCode = rgbProcessedStream->Start();
	if (retCode != eOK)
	{
		std::cout << "rgbProcessedStream stream failed to Start: " << string(retCode) << std::endl;
	}

	std::cout << "rgbProcessedStream Stream is started" << std::endl;

	rgbProcessedStream->Register(DisplayRGBAfterFDK);

	cout << "Press any key to exit" << endl;

	char ch;
	cin >> ch;

}

int  main(int argc, char* argv[])
{
    cout << "Inuitive sample application - receiving FDK frames " << endl;
    Execute();
    
    return 0;
}

