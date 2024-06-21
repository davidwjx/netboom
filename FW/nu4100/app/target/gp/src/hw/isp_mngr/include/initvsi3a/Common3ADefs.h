/******************************************************************************\
|* Copyright (c) 2020 by VeriSilicon Holdings Co., Ltd. ("VeriSilicon")       *|
|* All Rights Reserved.                                                       *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets of    *|
|* of VeriSilicon.  This is proprietary information owned or licensed by      *|
|* VeriSilicon.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of VeriSilicon.                                         *|
\******************************************************************************/
#ifndef VSI_3AV2_COMMON_
#define VSI_3AV2_COMMON_
#include "vsi3a_types.h"
#include "Matrix.h"

namespace vsi3av2 {

const int MAX_SUPPORT_ISPNUMBER = 16;


typedef Matrix<float> Matf;
typedef Matrix<double> Matd;
typedef Matrix<uchar> Matu8;

template<typename T> class Point {
public:
    T x, y;
    Point() {}
    virtual ~Point() {}
    Point(T _x, T _y) : x(_x), y(_y) {}
    Point(const Point<T>& r) : x(r.x), y(r.y) {}
};

class LimitedScalar3f 
{
public:
    float curVal, maxVal, minVal;
    bool within(const float val) { return (val >= minVal && val <= maxVal); }
    void clip(float& val) { val = min(maxVal, max(minVal, val)); }
    const float get() { return curVal; }
    void set(const float val) { curVal = val; clip(curVal);}
};

/* mask for negative */
inline float fix2float(uint ival, uint mask, float ratio) {
    return (ival&mask)?(((ival^(mask-1))&(mask-1))+1)/-ratio:ival/ratio;
}

/* mask for negative */
inline uint float2fix(float fval, uint mask, float ratio) {
    float f = fval * ratio;
    return (f>0)?((uint)(f+0.5))&(mask-1):(~((uint)(-f+0.5))+1)&(mask-1);
}

struct ZFilterParams {
    float L, G, Q, R, P;
};

float ZFilter(ZFilterParams* params, float fVal);

bool interpolate(const vector<float>& vx, const vector<float>& vy, float x, float& y);
bool interpolate(const float* vx, const float* vy, int num, float x, float* y);
bool interpolateMatrix(const vector<float>& vx, const vector<vector<float>>& vy, float x, Matf& y);
float solveCurve(vector<float>& vx, vector<float>& vy);

}

#endif
