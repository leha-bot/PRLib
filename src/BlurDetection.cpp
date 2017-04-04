/*
   UFOCR - User-Friendly OCR
   Copyright (C) 2017 Alexander Zaitsev <zamazan4ik@tut.by>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "BlurDetection.hpp"

#include "opencv2/imgproc.hpp"

// OpenCV port of 'LAPM' algorithm (Nayar89)
double LAPM_Algo(const cv::Mat& src)
{
    cv::Mat M = (cv::Mat_<double>(3, 1) << -1, 2, -1);
    cv::Mat G = cv::getGaussianKernel(3, -1, CV_64F);

    cv::Mat Lx;
    cv::sepFilter2D(src, Lx, CV_64F, M, G);

    cv::Mat Ly;
    cv::sepFilter2D(src, Ly, CV_64F, G, M);

    cv::Mat FM = cv::abs(Lx) + cv::abs(Ly);

    double focusMeasure = cv::mean(FM).val[0];
    return focusMeasure;
}

// OpenCV port of 'LAPV' algorithm (Pech2000)
double LAPV_Algo(const cv::Mat& src)
{
    cv::Mat lap;
    cv::Laplacian(src, lap, CV_64F);

    cv::Scalar mu, sigma;
    cv::meanStdDev(lap, mu, sigma);

    double focusMeasure = sigma.val[0] * sigma.val[0];
    return focusMeasure;
}

// OpenCV port of 'TENG' algorithm (Krotkov86)
double TENG_Algo(const cv::Mat& src, int ksize)
{
    cv::Mat Gx, Gy;
    cv::Sobel(src, Gx, CV_64F, 1, 0, ksize);
    cv::Sobel(src, Gy, CV_64F, 0, 1, ksize);

    cv::Mat FM = Gx.mul(Gx) + Gy.mul(Gy);

    double focusMeasure = cv::mean(FM).val[0];
    return focusMeasure;
}

// OpenCV port of 'GLVN' algorithm (Santos97)
double GLVN_Algo(const cv::Mat& src)
{
    cv::Scalar mu, sigma;
    cv::meanStdDev(src, mu, sigma);

    double focusMeasure = (sigma.val[0] * sigma.val[0]) / mu.val[0];
    return focusMeasure;
}

#include <QDebug>

bool IPL::isBlurred(const cv::Mat& src, IPL::BlurDetectionAlgo algo)
{
    //Begin test
    qDebug() << "LAPM: " << LAPM_Algo(src);
    qDebug() << "LAPV: " << LAPV_Algo(src);
    qDebug() << "TENG: " << TENG_Algo(src, 5);
    qDebug() << "GLVN: " << GLVN_Algo(src);
    //End test

    //TODO: find constants for blurring check for every algorithm. Write supertest, powered by all algorithms
    switch (algo)
    {
        case BlurDetectionAlgo::LAPM:
            return LAPM_Algo(src);
        case BlurDetectionAlgo::LAPV:
            return LAPV_Algo(src);
        case BlurDetectionAlgo::TENG:
            return TENG_Algo(src, 5);
        case BlurDetectionAlgo::GLVN:
            return GLVN_Algo(src);
    }
}

