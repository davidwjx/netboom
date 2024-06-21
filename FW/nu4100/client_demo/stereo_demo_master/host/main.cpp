#include "InuSensorExt.h"
#include "UserDefinedStream.h"
#include "GeneralFrame.h"
#include "ImageStream.h"

#include "inu_types.h"
#include "../common/stereo_demo.h"
#include "stereo_demo_example.h"

#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>

using namespace InuDev;
using namespace std;

std::shared_ptr<InuDev::CInuSensorExt>  inuSensor;
std::shared_ptr<InuDev::CUserDefinedStream> fdkStream;
std::shared_ptr<InuDev::CImageStream> FEPreprocessedStream;
std::shared_ptr<InuDev::CImageStream> FE2PreprocessedStream;

int fe_img_HEIGHT = 0;
int fe_img_WIDTH = 0;
static char current_directory[200];
static const bool DISPLAY = true;

//#define DEBUG_INJECT

#define SSD_NET_ID          0
#define CLASSIFICATION_MAX_CLASSES  1000
#define MAX_LABEL_LENGTH            30
#define OBJECT_DETECTION_NAME_LEN MAX_LABEL_LENGTH
#define OBJECT_MAX_NUMBER           20
#define CONF_THRESHOLD              0.4F

static char classes[CLASSIFICATION_MAX_CLASSES][MAX_LABEL_LENGTH];
static int number_of_classes = 0;
static char CNN_bin_dir[200];
static string qdata_dir_name = "SSD_NET";

bool flipped = true;
static char object_names[2][OBJECT_MAX_NUMBER][OBJECT_DETECTION_NAME_LEN];
static int object_min_x[2][OBJECT_MAX_NUMBER], object_max_x[2][OBJECT_MAX_NUMBER], object_min_y[2][OBJECT_MAX_NUMBER], object_max_y[2][OBJECT_MAX_NUMBER];
static int object_label[2][OBJECT_MAX_NUMBER];
static float object_confidence[2][OBJECT_MAX_NUMBER];
static int num_of_objects[2] = { 0 };

#define MAX_COLORS 10
static CvScalar class_colors[MAX_COLORS]{
    { 0, 0, 255 }, { 255, 0, 255 }, { 0, 255, 0 }, { 255, 0, 0 }, { 0, 255, 255 },
    { 192, 192, 192 }, { 0, 0, 128 }, { 128, 0, 128 }, { 128, 0, 0 }, { 0, 128, 128 }
};

/* load_labels */
bool load_labels(int net_id, string classes_file)
{
    FILE* fd;
    bool status = true;
    char line[MAX_LABEL_LENGTH], * line_p;

    fd = fopen(classes_file.c_str(), "r");
    if (fd) {
        printf("Reading class information from %s\n", classes_file.c_str());
        line_p = fgets(line, sizeof(line), fd);
        number_of_classes = 0;
        while (line_p) {
            if (line[0] != '#') {
                if (number_of_classes >= CLASSIFICATION_MAX_CLASSES) {
                    printf("classification_load_classes: too many classes in %s\n", classes_file.c_str());
                    fclose(fd);
                    return true;
                }

                line[strlen(line) - 1] = '\0'; // Remove the CR

                strcpy(classes[number_of_classes], line);
                number_of_classes++;
            }
            line_p = fgets(line, sizeof(line), fd);
        }
        fclose(fd);
        printf("Loaded %d class names\n", number_of_classes);
    }
    else {
        printf("Failed to load %s\n", classes_file.c_str());
        status = false;
    }

    return (status);
}

// Load Lables and Networks
bool LoadCNNNetworks()
{
    CInuError err(eOK);
    string net_id;
    string net_file, classes_file;
    bool rc;

#ifdef _MSC_VER
    GetCurrentDirectoryA(sizeof(CNN_bin_dir), CNN_bin_dir);
#else
    getcwd(CNN_bin_dir, sizeof(CNN_bin_dir));
#endif

    CCnnLoadParams cnn_load_params;
    // Load SSD Network
    net_id = to_string(SSD_NET_ID);
#ifdef _MSC_VER
    net_file = "c:\\Program Files\\Inuitive\\InuDev\\config\\AI4100\\cnn_bins\\nn_bin_mobilenet_ssd_varjo_v2.bin";
#else
    net_file = build_file_name(2, CNN_bin_dir, binNames[0].c_str());
#endif
    classes_file = build_file_name(3, CNN_bin_dir, qdata_dir_name.c_str(), "labels.txt");

    rc = load_labels(SSD_NET_ID, classes_file);

    if (!rc)
    {
        return false;
    }

    cnn_load_params.NetworkID = net_id;
    cnn_load_params.NetworkFilename = net_file;

    cnn_load_params.CnnNetworkEngineType = CCnnLoadParams::eSynopsys;
    cnn_load_params.CnnChannelSwap = CCnnLoadParams::CNN_NETWORK_CHANNEL_SWAP;
    cnn_load_params.PipeDepth = 1;
    cnn_load_params.ProcesssingFlags = 2; //convert to 888

    printf("Loading Net:%s  net_file:%s\n", net_id.c_str(), net_file.c_str());
    err = inuSensor->LoadCnnNetworks(cnn_load_params);

    if (err != eOK)
    {
        printf("Failed to load net: %s  file:%s\n", net_id.c_str(), net_file.c_str());
        return false;
    }
    else
    {
        printf("net: %s  file:%s loaded succesfully\n", net_id.c_str(), net_file.c_str());
    }
    return true;
}

/* ssd_parser */
void ssd_parser(unsigned char* blob, int channel)
{
    float total = 0;
    BBOX* boxes;
    int valid_count;

    blob += (channel * STEREO_OUTPUT_CNN_BLOB);

    valid_count = ((int*)blob)[0];
    boxes = (BBOX*)(blob + 4);

    num_of_objects[channel] = 0;

    //cout << "valid_count" << valid_count << endl;

    for (int i = 0; i < valid_count; i++)
    {
        if (boxes[i].confidence > CONF_THRESHOLD)
        {
            /* Check for valid label value */
            if (boxes[i].label >= number_of_classes || boxes[i].label < 0)
            {
                cout << "ssd_parser: Invalid label: " << boxes[i].label << "at index" << i << endl;
                break;
            }

            object_label[channel][num_of_objects[channel]] = boxes[i].label;
            object_min_x[channel][num_of_objects[channel]] = (int)(boxes[i].x_min * fe_img_WIDTH);
            object_max_x[channel][num_of_objects[channel]] = (int)(boxes[i].x_max * fe_img_WIDTH);
            object_min_y[channel][num_of_objects[channel]] = (int)(boxes[i].y_min * fe_img_HEIGHT);
            object_max_y[channel][num_of_objects[channel]] = (int)(boxes[i].y_max * fe_img_HEIGHT);
            object_confidence[channel][num_of_objects[channel]] = boxes[i].confidence;
            strcpy(object_names[channel][num_of_objects[channel]], classes[boxes[i].label]);

            num_of_objects[channel]++;
        }
    }
}

/* handle_classification_image */
void handle_classification_image(cv::Mat& webcam_mat, int channel)
{
    int obj_ind, x_min, x_max;
    char name_line[100];
    cv::Point pt1, pt2, text_pos;
    int thickness = 2;
    int text_thickness = 2;
    float text_scale = 2;
    int line_type = 8;
    int shift = 0;
    int base_line = 0;
    cv::Size textSize;
    CvScalar text_color = { 0, 0, 0 };

    mutex_lock(MUTEX_DISPLAY);

    for (obj_ind = 0; obj_ind < num_of_objects[channel]; obj_ind++) {
        /* Add the name and the confidence */
        sprintf(name_line, "%s (%2.3f)", object_names[channel][obj_ind], object_confidence[channel][obj_ind]);

        /* If the frame is flipped, we need to adjust the X coordinates */
        if (flipped) {
            x_max = webcam_mat.cols - object_min_x[channel][obj_ind];
            x_min = webcam_mat.cols - object_max_x[channel][obj_ind];
        }
        else {
            x_min = object_min_x[channel][obj_ind];
            x_max = object_max_x[channel][obj_ind];
        }

        /* If face too high, put the text below the box, else put it above */
        if (object_min_y[channel][obj_ind] <= 50) {
            if (object_max_y[channel][obj_ind] > webcam_mat.rows - 50) {
                text_pos = cv::Point(x_min, object_max_y[channel][obj_ind] - 10); // Frame too big. Put the text inside the box
            }
            else {
                text_pos = cv::Point(x_min, object_max_y[channel][obj_ind] + 20); // was 10
            }
        }
        else {
            text_pos = cv::Point(x_min, object_min_y[channel][obj_ind] - 10); // was 5
        }

        textSize = cv::getTextSize(name_line, cv::FONT_HERSHEY_PLAIN, text_scale, text_thickness, &base_line);
        textSize.height += 5;
        pt1 = text_pos + cv::Point(0, -textSize.height);
        pt2 = text_pos + cv::Point(textSize.width, base_line);
        cv::rectangle(webcam_mat, pt1, pt2, class_colors[object_label[channel][obj_ind] % MAX_COLORS], CV_FILLED, line_type, shift);


        cv::putText(webcam_mat,
            name_line,
            text_pos,
            cv::FONT_HERSHEY_PLAIN, // Font
            text_scale, // Scale. 2.0 = 2x bigger... was 0.5
            text_color, //obj_class_colors[obj_ind], // Color
            text_thickness); // Thickness

        /* Put the frame around the objects */
        pt1 = cv::Point(x_min, object_min_y[channel][obj_ind]);
        pt2 = cv::Point(x_max, object_max_y[channel][obj_ind]);
        cv::rectangle(webcam_mat, pt1, pt2, class_colors[object_label[channel][obj_ind] % MAX_COLORS], thickness, line_type, shift);
    }
    mutex_unlock(MUTEX_DISPLAY);
}

/* FdkCallback */
void  FdkCallback(std::shared_ptr<InuDev::CUserDefinedStream> iStream,      // Parent Stream
    std::shared_ptr<const InuDev::CGeneralFrame> iFrame,                    // Acquired frame
    InuDev::CInuError err)                                                  // Error code (eOK if frame was successfully acquired)
{
    unsigned char* blob;

    if (err == eOK)
    {
        blob = (unsigned char*)iFrame->GetData();
        //printf("blob len:%d\n", iFrame->GetDataSize());
        mutex_lock(MUTEX_DISPLAY);
        ssd_parser((unsigned char*)blob, LEFT_CHANNEL);
        ssd_parser((unsigned char*)blob, RIGHT_CHANNEL);
        printf("process blob output vdsp1: 0x%X vdsp2: 0x%X\n", *(unsigned int*)(blob + STEREO_OUTPUT_CNN_BLOB_MAX_SIZE), *(unsigned int*)(blob + STEREO_OUTPUT_CNN_BLOB_MAX_SIZE + STEREO_OUTPUT_PROCESS_BLOB));
        mutex_unlock(MUTEX_DISPLAY);

    }
    else
    {
        if (int(err) != eTimeoutError) {
            printf("FdkCallback Error: %d - %s\n", int(err), string(err).c_str());
        }
    }
}

// This callback is invoked when an image frame is ready 
void  FEPreProcessedStreamCallback(std::shared_ptr<InuDev::CImageStream> iStream,      // Parent Stream
    std::shared_ptr<const InuDev::CImageFrame> iFrame,                                  // Acquired frame
    InuDev::CInuError retCode)                                                          // Error code (eOK if frame was successfully acquired)
{
    if (retCode != InuDev::eOK)
    {
        std::cout << "Error in receiving PreProcessed frame: " << std::hex << int(retCode) << " " << std::string(retCode) << std::endl;
        return;
    }

    if (iFrame->Valid == false)
    {
        std::cout << "Frame  " << iFrame->FrameIndex << " is invalid" << std::endl;
        return;
    }

    // These 2 variables are used to calculate the FPS
    static uint64_t  firstIndex = iFrame->FrameIndex;
    static uint64_t  firstTS = iFrame->Timestamp;

    static int counter = 0;
    if (!(++counter % 100))
    {
        cout << "pre process stream FPS: " << double(iFrame->FrameIndex - firstIndex) / (iFrame->Timestamp - firstTS) * 1e9 << endl;
    }

    
    cv::Mat img;
    fe_img_HEIGHT = iFrame->Height();
    fe_img_WIDTH = iFrame->Width();

    img = cv::Mat(fe_img_HEIGHT, fe_img_WIDTH, CV_8UC1, (uchar*)iFrame->GetData());
    // Use OpenCV to display the image 
    cv::namedWindow("FE Pre Processed Stream", cv::WINDOW_NORMAL);
    cv::Mat img3(fe_img_HEIGHT, fe_img_WIDTH, CV_8UC3);
    cv::cvtColor(img, img3, CV_GRAY2RGB);
    handle_classification_image(img3, LEFT_CHANNEL);
    cv::imshow("FE Pre Processed Stream", img3);
    cv::waitKey(1);
        
}

void  FE2PreProcessedStreamCallback(std::shared_ptr<InuDev::CImageStream> iStream,      // Parent Stream
    std::shared_ptr<const InuDev::CImageFrame> iFrame,                                  // Acquired frame
    InuDev::CInuError retCode)                                                          // Error code (eOK if frame was successfully acquired)
{
    if (retCode != InuDev::eOK)
    {
        std::cout << "Error in receiving PreProcessed frame: " << std::hex << int(retCode) << " " << std::string(retCode) << std::endl;
        return;
    }

    if (iFrame->Valid == false)
    {
        std::cout << "Frame  " << iFrame->FrameIndex << " is invalid" << std::endl;
        return;
    }

    // These 2 variables are used to calculate the FPS
    static uint64_t  firstIndex = iFrame->FrameIndex;
    static uint64_t  firstTS = iFrame->Timestamp;

    static int counter = 0;
    if (!(++counter % 100))
    {
        cout << "pre process stream FPS: " << double(iFrame->FrameIndex - firstIndex) / (iFrame->Timestamp - firstTS) * 1e9 << endl;
    }


    cv::Mat img;
    fe_img_HEIGHT = iFrame->Height();
    fe_img_WIDTH = iFrame->Width();

    img = cv::Mat(fe_img_HEIGHT, fe_img_WIDTH, CV_8UC1, (uchar*)iFrame->GetData());
    // Use OpenCV to display the image 
    cv::namedWindow("FE2 Pre Processed Stream", cv::WINDOW_NORMAL);
    cv::Mat img3(fe_img_HEIGHT, fe_img_WIDTH, CV_8UC3);
    cv::cvtColor(img, img3, CV_GRAY2RGB);
    handle_classification_image(img3, RIGHT_CHANNEL);
    cv::imshow("FE2 Pre Processed Stream", img3);
    cv::waitKey(1);

}

void InjectStereoImage()
{

    // Create User Defined stream:
    fdkStream = inuSensor->CreateUserDefinedStream("Sout_stereo_fdk_inject");

    //init & start FDK stream
    InuDev::CInuError retCode = fdkStream->Init();

    if (retCode != InuDev::eOK)
    {
        std::cout << "FDK Stream initiation error: " << std::hex << int(retCode) << " - " << std::string(retCode) << std::endl;
        return;
    }
    std::cout << "FDK Stream is initialized" << std::endl;

    retCode = fdkStream->Start();

    STEREO_DEMO_hostGpMsgStructT hostGPmsg;
    memset(&hostGPmsg, 0, sizeof(STEREO_DEMO_hostGpMsgStructT));
    hostGPmsg.algInfo.enable = 1;

    retCode = inuSensor->ControlFDK((const unsigned char*)&hostGPmsg, sizeof(hostGPmsg), "FDK_STEREO_DEMO_INJECT");
    if (retCode != eOK)
    {
        std::cout << "Control FDK failed: " << string(retCode) << std::endl;
        return;
    }

    // Register to start receiving all FDK streams
    fdkStream->Register(FdkCallback);

    //write input images to fdk
    InuDev::CImageDescriptor nImgDesc;

    /* Populate the descriptor */
    nImgDesc.format = FW_NUCFG_FORMAT_RAW8_E;
    nImgDesc.width = 800;
    nImgDesc.bufferHeight = nImgDesc.height = 800;
    nImgDesc.x = 0;
    nImgDesc.y = 0;
    nImgDesc.stride = 800;
    nImgDesc.bitsPerPixel = 8;
    nImgDesc.realBitsMask = 0;
    nImgDesc.numInterleaveImages = 1;
    nImgDesc.interMode = FW_NUCFG_INTER_MODE_FRAME_BY_FRAME_E;

    FILE* raw = fopen("face.raw", "rb");
    if (raw == NULL)
    {
        printf("Failed to read raw image\n");
        return;
    }
    int size = nImgDesc.width * nImgDesc.height;
    unsigned char* bufP[2];
    bufP[0] = (unsigned char*)malloc(size*2);
    fread((void*)(bufP[0]), sizeof(unsigned char), size*2, raw);
    fclose(raw);
    raw = fopen("dog.raw", "rb");
    if (raw == NULL)
    {
        printf("Failed to read raw image\n");
        return;
    }
    bufP[1] = (unsigned char*)malloc(size * 2);
    fread((void*)(bufP[1]), sizeof(unsigned char), size * 2, raw);
    fclose(raw);

    for (int i = 0; i < 60; i++)
    {
        retCode = fdkStream->WriteData("FDK_inject_left_stream_in_func", nImgDesc, bufP[i & 1], size);
        if (retCode != eOK)
        {
            printf("Failed to Inject left image\n");
        }
        retCode = fdkStream->WriteData("FDK_inject_right_stream_in_func", nImgDesc, bufP[i & 1] +size, size);
        if (retCode != eOK)
        {
            printf("Failed to Inject right image\n");
        }
        Sleep(1000);

        //display results
        cv::Mat img;
        fe_img_HEIGHT = nImgDesc.height;
        fe_img_WIDTH = nImgDesc.width;

        img = cv::Mat(fe_img_HEIGHT, fe_img_WIDTH, CV_8UC1, bufP[i & 1]);
        // Use OpenCV to display the image 
        cv::namedWindow("Inject Left", cv::WINDOW_NORMAL);
        cv::Mat img3Left(fe_img_HEIGHT, fe_img_WIDTH, CV_8UC3);
        cv::cvtColor(img, img3Left, CV_GRAY2RGB);
        handle_classification_image(img3Left, LEFT_CHANNEL);
        cv::imshow("Inject Left", img3Left);

        img = cv::Mat(fe_img_HEIGHT, fe_img_WIDTH, CV_8UC1, bufP[i & 1] +size);
        // Use OpenCV to display the image 
        cv::namedWindow("Inject Right", cv::WINDOW_NORMAL);
        cv::Mat img3Right(fe_img_HEIGHT, fe_img_WIDTH, CV_8UC3);
        cv::cvtColor(img, img3Right, CV_GRAY2RGB);
        handle_classification_image(img3Right, RIGHT_CHANNEL);
        cv::imshow("Inject Right", img3Right);

        cv::waitKey(1);

    }

}

void runFromSensor()
{

    // Create User Defined stream:
    fdkStream = inuSensor->CreateUserDefinedStream("Sout_stereo_fdk");

    // Create RGB stream which will provide images to the FDK
    FEPreprocessedStream = inuSensor->CreateImageStream(2);

    // Create RGB stream which will provide images to the FDK
    FE2PreprocessedStream = inuSensor->CreateImageStream(3);

    //init & start FDK stream
    InuDev::CInuError retCode = fdkStream->Init();

    if (retCode != InuDev::eOK)
    {
        std::cout << "FDK Stream initiation error: " << std::hex << int(retCode) << " - " << std::string(retCode) << std::endl;
        return;
    }
    std::cout << "FDK Stream is initialized" << std::endl;

    retCode = fdkStream->Start();

    STEREO_DEMO_hostGpMsgStructT hostGPmsg;
    memset(&hostGPmsg, 0, sizeof(STEREO_DEMO_hostGpMsgStructT));
    hostGPmsg.algInfo.enable = 1;

    retCode = inuSensor->ControlFDK((const unsigned char*)&hostGPmsg, sizeof(hostGPmsg), "FDK_STEREO_DEMO_FE");
    if (retCode != eOK)
    {
        std::cout << "Control FDK failed: " << string(retCode) << std::endl;
        return;
    }

    // Configure and start the pre-processed stream 
    retCode = FEPreprocessedStream->Init(CImageStream::EOutputFormat::eRaw);
    if (retCode != InuDev::eOK)
    {
        std::cout << "FE initiation error: " << std::hex << int(retCode) << " - " << std::string(retCode) << std::endl;
        return;
    }
    std::cout << "FE Stream is initialized" << std::endl;

    retCode = FEPreprocessedStream->Start();
    if (retCode != InuDev::eOK)
    {
        std::cout << "Start error: " << std::hex << int(retCode) << " - " << std::string(retCode) << std::endl;
        return;
    }

    FEPreprocessedStream->Register(FEPreProcessedStreamCallback);

    // Configure and start the pre-processed 2nd stream 
    retCode = FE2PreprocessedStream->Init(CImageStream::EOutputFormat::eRaw);
    if (retCode != InuDev::eOK)
    {
        std::cout << "FE initiation error: " << std::hex << int(retCode) << " - " << std::string(retCode) << std::endl;
        return;
    }
    std::cout << "FE 2nd Stream is initialized" << std::endl;

    retCode = FE2PreprocessedStream->Start();
    if (retCode != InuDev::eOK)
    {
        std::cout << "Start error: " << std::hex << int(retCode) << " - " << std::string(retCode) << std::endl;
        return;
    }

    FE2PreprocessedStream->Register(FE2PreProcessedStreamCallback);

    // Register to start receiving all FDK streams
    fdkStream->Register(FdkCallback);
}

// Execute the demo 
void Execute()
{
   // The SW graph and HW graph should be overwritten in this demo
    InuDev::CDeviceParamsExt inuSensorParams;
    CHwInformation oHWInfo;

   static char current_directory[200];

   mutex_create(MUTEX_DISPLAY);

#ifdef _MSC_VER
   GetCurrentDirectoryA(sizeof(current_directory), current_directory);
#else
   getcwd(current_directory, sizeof(current_directory));
#endif
   //inuSensorParams.GraphXmlPath = build_file_name(2, current_directory, "sw_graph_stereo_demo.xml");
   //inuSensorParams.mHwGraphXmlPath = build_file_name(2, current_directory, "nu4100_boot10065_FE_8bp.xml");
   inuSensorParams.GraphXmlPath = build_file_name(2, current_directory, "sw_graph_boot10065_Master.xml");
   inuSensorParams.mHwGraphXmlPath = build_file_name(2, current_directory, "nu4100_boot10065_Master.xml");

   // Creation of CInuSensor object
   inuSensor = InuDev::CInuSensorExt::Create();

   // Initiate the sensor - it must be call before any access to the sensor. Sensor will start working in low power. 
   //inuSensorParams.FPS = 50;
   InuDev::CInuError retCode = inuSensor->Init(oHWInfo, inuSensorParams);
   if (retCode != InuDev::eOK)
   {
      std::cout << "Failed to connect to Inuitive Sensor. Error: " << std::hex << int(retCode) << " - " << std::string(retCode) << std::endl;
      return;
   }

   std::cout << "Connected to Sensor" << std::endl;

   // Start acquiring frames - it must be call before starting acquiring any type of frames (depth, video, head, etc.)
   retCode = inuSensor->Start();
   if (retCode != InuDev::eOK)
   {
      std::cout << "Failed to start to Inuitive Sensor." << std::endl;
      return;
   }
   std::cout << "Sensor is started" << std::endl;      

   if (LoadCNNNetworks() != true)
       cout << "Error in Loading Networks " << endl;

#ifdef DEBUG_INJECT
   InjectStereoImage();
#else
   runFromSensor();
#endif

   // Start providing frames from the RGB channel to the FDK
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

