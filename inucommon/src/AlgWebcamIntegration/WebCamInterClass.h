#pragma once

#include <string>

#include <opencv2/opencv.hpp>

class WebCamInterClass
{
public:

    WebCamInterClass();

    // The following methods are essentially static...

    void init(
        cv::Size imsize,
        float focalX,
        float focalY,
        float *Oc,
        float *Kd,
        const cv::Mat &Rot,
        float *map_x,
        float *map_y, 
        bool isFisheye = false
    );

    void initCEVA(
        cv::Size imsize,
        float focalX,
        float focalY,
        float *Oc,
        float *Kd,
        const cv::Mat &Rot,
        cv::Mat &map_x,
        cv::Mat &map_y
    );

    void rectify_undistort(
        const cv::Mat &src,
        const cv::Mat &map_x,
        const cv::Mat &map_y,
        cv::Mat &dst
    );

    //LUT data
    //cv::Mat map_x; 
    //cv::Mat map_y;

protected:

    //Flag
    //bool LUT_Flag;
    
};
