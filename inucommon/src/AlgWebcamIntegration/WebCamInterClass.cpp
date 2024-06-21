#include <iostream>
#include "WebCamInterClass.h"

//bool WebCamInterClass::bInstanceFlag = false;
//WebCamInterClass* WebCamInterClass::pSingleWI = 0;

static void meshgrid(
    const cv::Mat &xgv, const cv::Mat &ygv,
    cv::Mat1i &X, cv::Mat1i &Y
)
{
    cv::repeat(xgv.reshape(1,1), (int)ygv.total(), 1, X);
    cv::repeat(ygv.reshape(1,1).t(), 1, (int)xgv.total(), Y);
}

// helper function (maybe that goes somehow easier)
static void meshgridMat(
    const cv::Range &xgv, const cv::Range &ygv,
    cv::Mat1i &X, cv::Mat1i &Y
)
{
    std::vector<int> t_x, t_y;
    for (int i = xgv.start; i <= xgv.end; i++) t_x.push_back(i);
    for (int i = ygv.start; i <= ygv.end; i++) t_y.push_back(i);
    meshgrid(cv::Mat(t_x), cv::Mat(t_y), X, Y);
}

static void meshgridMat(
    int xs, int xe, int xbin, int ys, int ye, int ybin,
    cv::Mat1i &X, cv::Mat1i &Y
)
{
    std::vector<int> t_x, t_y;
    for (int i = xs; i <= xe; i+=xbin) t_x.push_back(i);
    for (int i = ys; i <= ye; i+=ybin) t_y.push_back(i);
    meshgrid(cv::Mat(t_x), cv::Mat(t_y), X, Y);
}

void apply_fisheye_distortion(const cv::Mat &x, const cv::Mat &y, float* k, cv::Mat &xd, cv::Mat &yd)
{
    cv::Mat xsq, ysq;
    cv::multiply(x, x, xsq);
    cv::multiply(y, y, ysq);

    cv::Mat r2 = xsq + ysq;
    cv::Mat r;
    cv::sqrt(r2, r);

    cv::Mat theta(r.rows, r.cols, CV_32F);

    for (int row = 0; row < r.rows; row++)
        for (int col = 0; col < r.cols; col++)
            theta.at<float>(row, col) = atan(r.at<float>(row, col));
    
    cv::Mat theta2, theta3, theta4, theta5, theta6, theta7, theta8, theta9;
    cv::multiply(theta,theta,theta2);
    cv::multiply(theta2,theta,theta3);
    cv::multiply(theta2,theta2,theta4);
    cv::multiply(theta4,theta,theta5);
    cv::multiply(theta3,theta3,theta6);
    cv::multiply(theta6,theta,theta7);
    cv::multiply(theta4,theta4,theta8);
    cv::multiply(theta8,theta,theta9);

    cv::Mat theta_d = theta + k[0] * theta3 + k[1] * theta5 + k[2] * theta7 + k[3] * theta9;

    cv::Mat inv_r = 1 / r;
    cv::Mat mask = r <= 1e-8;
    inv_r.setTo(1.f, mask);
    cv::Mat cdist;
    cv::multiply(theta_d,inv_r,cdist);
    cdist.setTo(1.f, mask);
    
    cv::multiply(x,cdist,xd);
    cv::multiply(y,cdist,yd);
}

void apply_distortion(const cv::Mat &x,const cv::Mat &y, float* k, cv::Mat &xd, cv::Mat &yd)
{
    // Add distortion:

    cv::Mat xsq, ysq;
    cv::multiply(x, x, xsq);
    cv::multiply(y, y, ysq);

    cv::Mat r2 = xsq + ysq;

    cv::Mat r4,r6;
    cv::multiply(r2, r2, r4);
    cv::multiply(r4, r2, r6);

    //Radial distortion:
    cv::Mat cdist = 1 + k[0] * r2 + k[1] * r4 + k[4] * r6;

    cv::Mat xd1, yd1;
    cv::multiply(x, cdist, xd1);
    cv::multiply(y, cdist, yd1);

    //tangential distortion:
    cv::Mat a1,a2,a3;
    cv::multiply(x,y,a1,2);
    a2=r2+2*xsq;
    a3=r2+2*ysq;

    cv::Mat delta_x = k[2]*a1 + k[3]*a2; 
    cv::Mat delta_y = k[2]*a3 + k[3]*a1;

    xd = xd1 + delta_x;
    yd = yd1 + delta_y;
}

void create_WEB_LUTs_(cv::Size imsize, float focalX, float focalY, float *Oc, float* Kd, const cv::Mat &Rot, cv::Mat &x, cv::Mat &y)
{
    cv::Mat cameraMatrix = (cv::Mat_<float>(3,3) << focalX, 0, Oc[0], 0, focalY, Oc[1],0, 0, 1);
    cv::Mat distCoeffs = (cv::Mat_<double> (5,1) << Kd[0], Kd[1], Kd[3], Kd[4], Kd[2]);  
     
    cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, Rot, cameraMatrix, imsize, CV_16SC2, x, y);
}

void create_WEB_LUTs(cv::Size imsize, float focalX, float focalY, float *Oc, float* Kd, const cv::Mat &Rot, cv::Mat &x, cv::Mat &y, bool isFisheye)
{
    cv::Mat KK = (cv::Mat_<float>(3,3) << focalX, 0, Oc[0], 0, focalY, Oc[1],0, 0, 1);
    
    //[nr,nc] = size(I);
    cv::Mat1i X, Y;
    meshgridMat(cv::Range(0,imsize.width-1), cv::Range(0, imsize.height-1), X, Y); 

    int mn = imsize.width*imsize.height;
    cv::Mat1i px(1,mn,X.ptr<int>(0));
    cv::Mat1i py(1,mn,Y.ptr<int>(0));

    //FT::writeMat2(px, "D:/Users/genadiyv/Desktop/x.mat", "x", false);
    //FT::writeMat2(py, "D:/Users/genadiyv/Desktop/y.mat", "y", false);

    cv::Mat pxf; px.convertTo(pxf, CV_32F);
    cv::Mat pyf; py.convertTo(pyf, CV_32F);

    cv::Mat xyz(3,mn,CV_32F);
    pxf.copyTo(xyz.row(0));
    pyf.copyTo(xyz.row(1));

    cv::Mat ones_ = cv::Mat::ones(1,mn,CV_32F);
    ones_.copyTo(xyz.row(2));

    cv::Mat rays = KK.inv()*xyz;
    //FT::writeMat2(xyz, "D:\\Users\\genadiyv\\Desktop\\xyz.mat", "xyz",false);

    // Rotation: (or affine transformation):
    cv::Mat rays2 = Rot*rays;

    cv::Mat xn, yn;
    cv::divide(rays2.row(0), rays2.row(2), xn); 
    cv::divide(rays2.row(1), rays2.row(2), yn); 
    
    // Add distortion:
    cv::Mat xd_, yd_;
    if (!isFisheye)
        apply_distortion(xn, yn, Kd, xd_, yd_);
    else
        apply_fisheye_distortion(xn, yn, Kd, xd_, yd_);
    
    //std::cout << xd_ <<std::endl;
    
    // Reconvert in pixels:
    x = focalX*xd_+Oc[0];
    y = focalY*yd_+Oc[1];

    //FT::writeMat2(x, "D:/Users/genadiyv/Desktop/px.mat", "px", false);
    //FT::writeMat2(y, "D:/Users/genadiyv/Desktop/py.mat", "py", false);
}
/*
WebCamInterClass* WebCamInterClass::GetInstance()
{
    if(!bInstanceFlag)
    {
        pSingleWI = new WebCamInterClass();
        bInstanceFlag = true;
        return pSingleWI;
    }
    else
    {
        return pSingleWI;
    }
}
*/

void create_WEB_LUTsCEVA(cv::Size imsize, float focalX, float focalY, float *Oc, float* Kd, const cv::Mat &Rot, cv::Mat &x, cv::Mat &y)
{
    cv::Mat KK = (cv::Mat_<float>(3,3) << focalX, 0, Oc[0], 0, focalY, Oc[1],0, 0, 1);
    
    //[nr,nc] = size(I);
    cv::Mat1i X, Y;
    meshgridMat(0, imsize.width-1, 16, 0, imsize.height-1, 16, X, Y);

    int mn = X.rows*X.cols;
    cv::Mat1i px(1,mn,X.ptr<int>(0));
    cv::Mat1i py(1,mn,Y.ptr<int>(0));
    
    //FT::writeMat2(px, "D:/Users/genadiyv/Desktop/x.mat", "x", false);
    //FT::writeMat2(py, "D:/Users/genadiyv/Desktop/y.mat", "y", false);

    //writeMat(px, "D:/Users/genadiyv/Desktop/x.mat", "x", false);

    cv::Mat pxf; px.convertTo(pxf, CV_32F);
    cv::Mat pyf; py.convertTo(pyf, CV_32F);

    cv::Mat xyz(3,mn,CV_32F);
    pxf.copyTo(xyz.row(0));
    pyf.copyTo(xyz.row(1));

    cv::Mat ones_ = cv::Mat::ones(1,mn,CV_32F);
    ones_.copyTo(xyz.row(2));

    cv::Mat rays = KK.inv()*xyz;
    

    // Rotation: (or affine transformation):
    cv::Mat rays2 = Rot*rays;

    cv::Mat xn, yn;
    cv::divide(rays2.row(0), rays2.row(2), xn); 
    cv::divide(rays2.row(1), rays2.row(2), yn); 
    
    // Add distortion:
    cv::Mat xd_, yd_;
    apply_distortion(xn, yn, Kd, xd_, yd_);
    //std::cout << xd_ <<std::endl;
    
    // Reconvert in pixels:
    x = focalX*xd_+Oc[0];
    y = focalY*yd_+Oc[1];
}

WebCamInterClass::WebCamInterClass()
{
    //this->LUT_Flag = false;
}

void WebCamInterClass::init(
    cv::Size imsize,
    float focalX,
    float focalY,
    float* Oc,
    float* Kd,
    const cv::Mat& Rot,
    float* map_x,
    float* map_y,
    bool isFisheye
)
{
    cv::Mat x0, y0;
    //create_WEB_LUTsCEVA(imsize,focal, Oc, Kd, Rot, x0, y0);
    create_WEB_LUTs(imsize, focalX, focalY, Oc, Kd, Rot, x0, y0, isFisheye);

    cv::Mat Map_x = cv::Mat(imsize.height, imsize.width, CV_32F, x0.ptr<float>(0)).clone();
    cv::Mat Map_y = cv::Mat(imsize.height, imsize.width, CV_32F, y0.ptr<float>(0)).clone();

    // Glorified memcpy. Fix! (or eliminate...)

    float* Mx = Map_x.ptr<float>(0);
    float* My = Map_y.ptr<float>(0);
    float* mapx = map_x;
    float* mapy = map_y;

    for (int i = 0; i < imsize.height * imsize.width; i++)
    {
        *mapx = *Mx;
        *mapy = *My;
        Mx++; My++;
        mapx++; mapy++;
    }

    //this->map_x = x0.clone();
    //this->map_y = y0.clone();

    //this->LUT_Flag = true;
}

void WebCamInterClass::initCEVA(
    cv::Size imsize,
    float focalX,
    float focalY,
    float *Oc,
    float *Kd,
    const cv::Mat &Rot,
    cv::Mat &map_x,
    cv::Mat &map_y
)
{
    cv::Mat x0,y0;
    create_WEB_LUTsCEVA(imsize,focalX, focalY, Oc, Kd, Rot, x0, y0);
    //create_WEB_LUTs(imsize,focal, Oc, Kd, Rot, x0, y0);
    cv::Mat tempx, tempy;
    tempx = cv::Mat(imsize.height/16, imsize.width/16, CV_32F, x0.ptr<float>(0)).clone();
    tempy = cv::Mat(imsize.height/16, imsize.width/16, CV_32F, y0.ptr<float>(0)).clone();

    tempx = tempx*65536;
    tempy = tempy*65536;

    tempx.convertTo(map_x,CV_32S);
    tempy.convertTo(map_y,CV_32S);

    //this->LUT_Flag = true;
}

void WebCamInterClass::rectify_undistort(const cv::Mat& src, const cv::Mat& map_x, const cv::Mat& map_y, cv::Mat& dst)
{
    int t = src.type();

    switch (t)
    {
    case CV_8UC4:

        cv::remap(src, dst, map_x, map_y, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0, 0));
        return;

    case CV_8UC3:

        cv::remap(src, dst, map_x, map_y, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
        return;

    default:

        // now what?
        break;
    }
}
