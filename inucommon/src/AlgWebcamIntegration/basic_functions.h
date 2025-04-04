#pragma once

#include <iostream>
#include <stdint.h>
#include <fstream>

#include <opencv2/opencv.hpp>
// For openCV 4.5.1 (old types; TODO)
#include <opencv2/imgproc/types_c.h>

namespace FT
{
    inline int cvRodrigues2_(const CvMat* src, CvMat* dst, CvMat* jacobian)
    {
        int depth, elem_size;
        int i, k;
        double J[27];
        CvMat matJ = cvMat(3, 9, CV_64F, J);

        if (!CV_IS_MAT(src))
        {
            CV_Error(!src ? CV_StsNullPtr : CV_StsBadArg, "Input argument is not a valid matrix");
        }

        if (!CV_IS_MAT(dst))
        {
            CV_Error(!dst ? CV_StsNullPtr : CV_StsBadArg,
                "The first output argument is not a valid matrix");
        }

        depth = CV_MAT_DEPTH(src->type);
        elem_size = CV_ELEM_SIZE(depth);

        if (depth != CV_32F && depth != CV_64F)
        {
            CV_Error(CV_StsUnsupportedFormat, "The matrices must have 32f or 64f data type");
        }

        if (!CV_ARE_DEPTHS_EQ(src, dst))
        {
            CV_Error(CV_StsUnmatchedFormats, "All the matrices must have the same data type");
        }

        if (jacobian)
        {
            if (!CV_IS_MAT(jacobian))
            {
                CV_Error(CV_StsBadArg, "Jacobian is not a valid matrix");
            }

            if (!CV_ARE_DEPTHS_EQ(src, jacobian) || CV_MAT_CN(jacobian->type) != 1)
            {
                CV_Error(CV_StsUnmatchedFormats, "Jacobian must have 32fC1 or 64fC1 datatype");
            }

            if ((jacobian->rows != 9 || jacobian->cols != 3) &&
                (jacobian->rows != 3 || jacobian->cols != 9))
            {
                CV_Error(CV_StsBadSize, "Jacobian must be 3x9 or 9x3");
            }
        }

        if (src->cols == 1 || src->rows == 1)
        {
            double rx, ry, rz, theta;
            int step = src->rows > 1 ? src->step / elem_size : 1;

            if (src->rows + src->cols * CV_MAT_CN(src->type) - 1 != 3)
            {
                CV_Error(CV_StsBadSize, "Input matrix must be 1x3, 3x1 or 3x3");
            }

            if (dst->rows != 3 || dst->cols != 3 || CV_MAT_CN(dst->type) != 1)
            {
                CV_Error(CV_StsBadSize, "Output matrix must be 3x3, single-channel floating point matrix");
            }

            if (depth == CV_32F)
            {
                rx = src->data.fl[0];
                ry = src->data.fl[step];
                rz = src->data.fl[step * 2];
            }
            else
            {
                rx = src->data.db[0];
                ry = src->data.db[step];
                rz = src->data.db[step * 2];
            }

            theta = sqrt(rx * rx + ry * ry + rz * rz);

            if (theta < DBL_EPSILON)
            {
                cvSetIdentity(dst);

                if (jacobian)
                {
                    memset(J, 0, sizeof(J));
                    J[5] = J[15] = J[19] = -1;
                    J[7] = J[11] = J[21] = 1;
                }
            }
            else
            {
                const double I[] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };

                double c = cos(theta);
                double s = sin(theta);
                double c1 = 1. - c;
                double itheta = theta ? 1. / theta : 0.;

                rx *= itheta; ry *= itheta; rz *= itheta;

                double rrt[] = { rx * rx, rx * ry, rx * rz, rx * ry, ry * ry, ry * rz, rx * rz, ry * rz, rz * rz };
                double _r_x_[] = { 0, -rz, ry, rz, 0, -rx, -ry, rx, 0 };
                double R[9];
                CvMat matR = cvMat(3, 3, CV_64F, R);

                // R = cos(theta)*I + (1 - cos(theta))*r*rT + sin(theta)*[r_x]
                // where [r_x] is [0 -rz ry; rz 0 -rx; -ry rx 0]
                for (k = 0; k < 9; k++)
                {
                    R[k] = c * I[k] + c1 * rrt[k] + s * _r_x_[k];
                }

                cvConvert(&matR, dst);

                if (jacobian)
                {
                    double drrt[] = { rx + rx, ry, rz, ry, 0, 0, rz, 0, 0,
                        0, rx, 0, rx, ry + ry, rz, 0, rz, 0,
                        0, 0, rx, 0, 0, ry, rx, ry, rz + rz };

                    double d_r_x_[] = { 0, 0, 0, 0, 0, -1, 0, 1, 0,
                        0, 0, 1, 0, 0, 0, -1, 0, 0,
                        0, -1, 0, 1, 0, 0, 0, 0, 0 };

                    for (i = 0; i < 3; i++)
                    {
                        double ri = i == 0 ? rx : i == 1 ? ry : rz;
                        double a0 = -s * ri, a1 = (s - 2 * c1 * itheta) * ri, a2 = c1 * itheta;
                        double a3 = (c - s * itheta) * ri, a4 = s * itheta;

                        for (k = 0; k < 9; k++)
                        {
                            J[i * 9 + k] = a0 * I[k] + a1 * rrt[k] + a2 * drrt[i * 9 + k] +
                                a3 * _r_x_[k] + a4 * d_r_x_[i * 9 + k];
                        }
                    }
                }
            }
        }
        else if (src->cols == 3 && src->rows == 3)
        {
            double R[9], U[9], V[9], W[3], rx, ry, rz;
            CvMat matR = cvMat(3, 3, CV_64F, R);
            CvMat matU = cvMat(3, 3, CV_64F, U);
            CvMat matV = cvMat(3, 3, CV_64F, V);
            CvMat matW = cvMat(3, 1, CV_64F, W);
            double theta, s, c;
            int step = dst->rows > 1 ? dst->step / elem_size : 1;

            if ((dst->rows != 1 || dst->cols * CV_MAT_CN(dst->type) != 3) &&
                (dst->rows != 3 || dst->cols != 1 || CV_MAT_CN(dst->type) != 1))
            {
                CV_Error(CV_StsBadSize, "Output matrix must be 1x3 or 3x1");
            }

            cvConvert(src, &matR);

            if (!cvCheckArr(&matR, CV_CHECK_RANGE + CV_CHECK_QUIET, -100, 100))
            {
                cvZero(dst);

                if (jacobian)
                {
                    cvZero(jacobian);
                }

                return 0;
            }

            cvSVD(&matR, &matW, &matU, &matV, CV_SVD_MODIFY_A + CV_SVD_U_T + CV_SVD_V_T);
            cvGEMM(&matU, &matV, 1, 0, 0, &matR, CV_GEMM_A_T);

            rx = R[7] - R[5];
            ry = R[2] - R[6];
            rz = R[3] - R[1];

            s = sqrt((rx * rx + ry * ry + rz * rz) * 0.25);
            c = (R[0] + R[4] + R[8] - 1) * 0.5;
            c = c > 1. ? 1. : c < -1. ? -1. : c;
            theta = acos(c);

            if (s < 1e-5)
            {
                double t;

                if (c > 0)
                {
                    rx = ry = rz = 0;
                }
                else
                {
                    t = (R[0] + 1) * 0.5;
                    rx = sqrt(std::max(t, 0.));
                    t = (R[4] + 1) * 0.5;
                    ry = sqrt(std::max(t, 0.)) * (R[1] < 0 ? -1. : 1.);
                    t = (R[8] + 1) * 0.5;
                    rz = sqrt(std::max(t, 0.)) * (R[2] < 0 ? -1. : 1.);

                    if (fabs(rx) < fabs(ry) && fabs(rx) < fabs(rz) && (R[5] > 0) != (ry * rz > 0))
                    {
                        rz = -rz;
                    }

                    theta /= sqrt(rx * rx + ry * ry + rz * rz);
                    rx *= theta;
                    ry *= theta;
                    rz *= theta;
                }

                if (jacobian)
                {
                    memset(J, 0, sizeof(J));

                    if (c > 0)
                    {
                        J[5] = J[15] = J[19] = -0.5;
                        J[7] = J[11] = J[21] = 0.5;
                    }
                }
            }
            else
            {
                double vth = 1 / (2 * s);

                if (jacobian)
                {
                    double t, dtheta_dtr = -1. / s;
                    // var1 = [vth;theta]
                    // var = [om1;var1] = [om1;vth;theta]
                    double dvth_dtheta = -vth * c / s;
                    double d1 = 0.5 * dvth_dtheta * dtheta_dtr;
                    double d2 = 0.5 * dtheta_dtr;
                    // dvar1/dR = dvar1/dtheta*dtheta/dR = [dvth/dtheta; 1] * dtheta/dtr * dtr/dR

                    double dvardR[5 * 9] =
                    {
                        0, 0, 0, 0, 0, 1, 0, -1, 0,
                        0, 0, -1, 0, 0, 0, 1, 0, 0,
                        0, 1, 0, -1, 0, 0, 0, 0, 0,
                        d1, 0, 0, 0, d1, 0, 0, 0, d1,
                        d2, 0, 0, 0, d2, 0, 0, 0, d2
                    };

                    // var2 = [om;theta]
                    double dvar2dvar[] =
                    {
                        vth, 0, 0, rx, 0,
                        0, vth, 0, ry, 0,
                        0, 0, vth, rz, 0,
                        0, 0, 0, 0, 1
                    };

                    double domegadvar2[] =
                    {
                        theta, 0, 0, rx * vth,
                        0, theta, 0, ry * vth,
                        0, 0, theta, rz * vth
                    };

                    CvMat _dvardR = cvMat(5, 9, CV_64FC1, dvardR);
                    CvMat _dvar2dvar = cvMat(4, 5, CV_64FC1, dvar2dvar);
                    CvMat _domegadvar2 = cvMat(3, 4, CV_64FC1, domegadvar2);
                    double t0[3 * 5];
                    CvMat _t0 = cvMat(3, 5, CV_64FC1, t0);

                    cvMatMul(&_domegadvar2, &_dvar2dvar, &_t0);
                    cvMatMul(&_t0, &_dvardR, &matJ);

                    // transpose every row of matJ (treat the rows as 3x3 matrices)
                    CV_SWAP(J[1], J[3], t); CV_SWAP(J[2], J[6], t); CV_SWAP(J[5], J[7], t);
                    CV_SWAP(J[10], J[12], t); CV_SWAP(J[11], J[15], t); CV_SWAP(J[14], J[16], t);
                    CV_SWAP(J[19], J[21], t); CV_SWAP(J[20], J[24], t); CV_SWAP(J[23], J[25], t);
                }

                vth *= theta;
                rx *= vth; ry *= vth; rz *= vth;
            }

            if (depth == CV_32F)
            {
                dst->data.fl[0] = (float)rx;
                dst->data.fl[step] = (float)ry;
                dst->data.fl[step * 2] = (float)rz;
            }
            else
            {
                dst->data.db[0] = rx;
                dst->data.db[step] = ry;
                dst->data.db[step * 2] = rz;
            }
        }

        if (jacobian)
        {
            if (depth == CV_32F)
            {
                if (jacobian->rows == matJ.rows)
                {
                    cvConvert(&matJ, jacobian);
                }
                else
                {
                    float Jf[3 * 9];
                    CvMat _Jf = cvMat(matJ.rows, matJ.cols, CV_32FC1, Jf);
                    cvConvert(&matJ, &_Jf);
                    cvTranspose(&_Jf, jacobian);
                }
            }
            else if (jacobian->rows == matJ.rows)
            {
                cvCopy(&matJ, jacobian);
            }
            else
            {
                cvTranspose(&matJ, jacobian);
            }
        }

        return 1;
    }

    inline void Rodrigues_(cv::InputArray _src, cv::OutputArray _dst, cv::OutputArray _jacobian = cv::noArray())
    {
        cv::Mat src = _src.getMat();
        bool v2m = src.cols == 1 || src.rows == 1;
        _dst.create(3, v2m ? 3 : 1, src.depth());
        cv::Mat dst = _dst.getMat();
#if defined(CV_MAJOR_VERSION) && (CV_MAJOR_VERSION > 3)
        CvMat _csrc = cvMat(src), _cdst = cvMat(dst), _cjacobian;
#else
        CvMat _csrc = src, _cdst = dst, _cjacobian;
#endif
        if (_jacobian.needed())
        {
            _jacobian.create(v2m ? cv::Size(9, 3) : cv::Size(3, 9), src.depth());
#if defined(CV_MAJOR_VERSION) && (CV_MAJOR_VERSION > 3)
            _cjacobian = cvMat(_jacobian.getMat());
#else
            _cjacobian = _jacobian.getMat();
#endif
        }

        bool ok = cvRodrigues2_(&_csrc, &_cdst, _jacobian.needed() ? &_cjacobian : 0) > 0;

        if (!ok)
        {
            dst = cv::Scalar(0);
        }
    }
}
