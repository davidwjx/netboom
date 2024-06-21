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
#ifndef VSI_3AV2_MATRIX_
#define VSI_3AV2_MATRIX_
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <typeinfo>
#include <functional>

using namespace std;

namespace vsi3av2 {

#ifndef ALIGN_UP
#define ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align)-1))
#endif


template<typename _Tp> class Matrix
{
public:
    Matrix() {}
    virtual ~Matrix() { if (data) delete []data; }
    Matrix(int R, int C) { rows = R; cols = C; size = R*C; data = new _Tp[ALIGN_UP(size, 64)]; }
    Matrix(int R, int C, _Tp val) : Matrix(R, C) { for (int i = 0; i < size; i++) data[i] = val; }
    Matrix(int R, int C, const _Tp* raw_data) : Matrix(R, C) { memcpy(data, raw_data, size*sizeof(_Tp)); }
    Matrix(int R, int C, vector<_Tp>& v) : Matrix(R, C) { memcpy(data, &v[0], size*sizeof(_Tp)); }
    Matrix(const Matrix<_Tp>& r) : Matrix(r.rows, r.cols) { memcpy(data, r.data, size*sizeof(_Tp)); }
    inline Matrix<_Tp>& mul(const _Tp val) { for (int i = 0; i < size; i++) data[i] *= val; return *this; }
    inline Matrix<_Tp>& operator *= (const _Tp val){ for (int i = 0; i < size; i++) data[i] *= val; return *this; }
    inline Matrix<_Tp>& operator += (const _Tp val) { for (int i = 0; i < size; i++) data[i] += val; return *this; }
    inline Matrix<_Tp>& operator += (const Matrix<_Tp>& m) { for (int i = 0; i < size; i++) data[i] += m.data[i]; return *this; }
    inline Matrix<_Tp>& operator -= (const _Tp val) { for (int i = 0; i < size; i++) data[i] -= val; return *this; }
    inline Matrix<_Tp>& operator -= (const Matrix<_Tp>& m) { for (int i = 0; i < size; i++) data[i] -= m.data[i]; return *this; }
    inline Matrix<_Tp>& operator = (const _Tp val) { for (int i = 0; i < size; i++) data[i] = val; return *this; }
    inline Matrix<_Tp>& operator = (const _Tp* array) { if (data) { memcpy(data, array, size*sizeof(_Tp)); } return *this; }
    inline Matrix<_Tp>& operator = (const Matrix<_Tp>& r) { 
        if (rows != r.rows || cols != r.cols) { create(r.rows, r.cols); } memcpy(data, r.data, size*sizeof(_Tp)); return *this;
    }
    inline _Tp& operator[](int x) { if (x < 0 || x >= size) return zeroVal; return data[x]; }
    inline _Tp& at(int y, int x) { if (y < 0 || y >= rows || x < 0 || x >= cols) return zeroVal; return data[y*cols+x]; }

    inline void dump() {
        return;
#if 0
        printf("dump matrix\n");
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (typeid(mType) == typeid(float))
                    printf("%.3f, ", (float)at(i, j));
                else if (typeid(mType) == typeid(double))
                    printf("%.4f, ",  at(i, j));
                else
                    printf("%d, ",  (int)at(i, j));
            }
            printf("\n");
        }
        printf("\n");
#endif
    }

    inline double mean() {
        double sum = 0;
        for (int i = 0; i < size; i++) sum += data[i];
        return sum/size;
    }

    inline void create(int R, int C) {
        rows = R;
        cols = C;
        size = R*C;
        if (data)
            delete[] data;
        data = new _Tp[ALIGN_UP(size, 1024)];
    }

    inline void eye() {
        if (rows != cols) {
            printf("matrix eye rows != cols, %d %d\n", rows, cols);
            return;
        }
        int N = rows;
        memset(data, 0, size*sizeof(_Tp));
        for (int i = 0; i < N; i++) data[i*N+i] = 1;
    }

    inline void normalize() {
        double sum = 0;
        for (int i = 0; i < size; i++) sum += data[i];
        for (int i = 0; i < size; i++) data[i] /= sum;
    }

    inline void mul(Matrix<_Tp>& B, Matrix<_Tp>& dst) {
        dst.create(rows, B.cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < B.cols; j++) {
                _Tp tmp = 0;
                for (int k = 0; k < cols; k++) {
                    tmp += at(i, k) * B.at(k, j);  
                }
                dst.at(i, j) = tmp;
            }
        }
    }

    inline Matrix<_Tp>& operator *= (const Matrix<_Tp>& m) {
        Matrix<_Tp> dst;
        dst.create(rows, m.cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < m.cols; j++) {
                _Tp tmp = 0;
                for (int k = 0; k < cols; k++) {
                    tmp += at(i, k) *= m.at(k, j);  
                }
                dst.at(i, j) = tmp;
            }
        }
        memcpy(data, dst.data, size * sizeof(_Tp));
    }

    inline void add(Matrix<_Tp>& B, Matrix<_Tp>& dst) {
        dst.create(rows, cols);
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < B.cols; j++)
                dst.at(i, j) = at(i, j) + B.at(i, j);
    }

    inline void sub(Matrix<_Tp>& B, Matrix<_Tp>& dst) {
        dst.create(rows, cols);
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < B.cols; j++)
                dst.at(i, j) = at(i, j) - B.at(i, j);
    }

    inline void transpose(Matrix<_Tp>& dst) {
        dst.create(cols, rows);
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                dst.at(j, i) = at(i, j); 
    }

    bool invert(Matrix<_Tp>& dst) {
        if (rows != cols) {
            printf("matrix invert rows != cols, %d %d\n", rows, cols);
            return false;
        }
        int N = rows;
        Matrix<_Tp> A(N, N); Matrix<_Tp> B = *this; Matrix<_Tp> C(N, N);
        Matrix<_Tp> D(N, N, zeroVal); Matrix<_Tp> E(N, N, zeroVal); Matrix<_Tp> F;
        A.eye(); C.eye();
        for (int j = 0; j < N; j++) {
            _Tp maxVal = 0; int maxcol = j;
            for (int i = j; i < N; i++) {
                if (fabs(B.at(i, j)) > maxVal) {
                    maxVal = fabs(B.at(i, j));
                    maxcol = i;
                }
            }
            if (maxVal == 0.) return false;
            if (maxcol != j) {
                for (int k = 0; k < N; k++) std::swap(B.at(j, k), B.at(maxcol, k));
                for (int k = 0; k < j; k++) std::swap(A.at(j, k), A.at(maxcol, k));
                for (int k = 0; k < N; k++) std::swap(C.at(j, k), C.at(maxcol, k));
            }
            for (int i = j+1; i < N; i++) {
                A.at(i, j) = B.at(i, j) / B.at(j, j);
                for (int k = j; k < N; k++) B.at(i, k) = B.at(i, k) - A.at(i, j) * B.at(j, k);
            }
        }
        for (int i = 0; i < N; i++) {
            D.at(i, i) = 1;
            for (int j = i+1; j < N; j++)
                for (int k = i; k <= j-1; k++)
                    D.at(j, i) -= A.at(j, k) * D.at(k, i);
        }
        for (int i = 0; i < N; i++) {
            E.at(i, i) = (_Tp)(1.0 / B.at(i, i));
            for (int j = i-1; j >= 0; j--) {
                double sum = 0;
                for (int k = j+1; k <= i; k++) sum = sum + B.at(j, k) * E.at(k, i);
                E.at(j, i) = (_Tp)(-sum / B.at(j, j));
            }
        }
        E.mul(D, F);
        F.mul(C, dst);
        return true;
    }

public:
    int rows = 0, cols = 0, size = 0;
    _Tp* data = nullptr;
    _Tp zeroVal = 0;
    using mType = _Tp;
};

}

#endif
