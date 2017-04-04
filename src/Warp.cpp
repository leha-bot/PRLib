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

#include "Warp.hpp"

#include "opencv2/imgproc.hpp"

void IPL::warpCrop(const cv::Mat& sourceImg,
              cv::Mat& destImg,
              const int x0, const int y0,
              const int x1, const int y1,
              const int x2, const int y2,
              const int x3, const int y3,
              const int borderMode /*= cv::BORDER_CONSTANT*/,
              const cv::Scalar& borderValue /*= cv::Scalar()*/)
{
    const double side1 = sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
    const double side2 = sqrt((x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2));
    const double side3 = sqrt((x3 - x0) * (x3 - x0) + (y3 - y0) * (y3 - y0));
    const double side4 = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

    double aspect12 = side1 / side2;
    double aspect34 = side3 / side4;

    if (aspect12 < 1.0)
    {
        aspect12 = 1.0 / aspect12;
    }

    if (aspect34 < 1.0)
    {
        aspect34 = 1.0 / aspect34;
    }

    long bitmapWidth = cvRound(std::max(side1, side2) * aspect34);
    long bitmapHeight = cvRound(std::max(side3, side4) * aspect12);

    // bitmapWidth/bitmapHeight can be easily more than input w/h.
    // It leads to using more memory than expected.
    // We have to limit the cropped image size by the source image's size.
    // Special case: we can get size more than max texture size (80 thousands).

    if (side1 > bitmapWidth || side2 > bitmapWidth
        || side3 > bitmapHeight || side4 > bitmapHeight)
    {
        double ratio = bitmapWidth > bitmapHeight ? sourceImg.cols / bitmapWidth : sourceImg.rows / bitmapHeight;

        bitmapWidth = cvRound(bitmapWidth * ratio);
        bitmapHeight = cvRound(bitmapHeight * ratio);
    }

    float srcBuff[] = {static_cast<float>(x0), static_cast<float>(y0),
                       static_cast<float>(x1), static_cast<float>(y1),
                       static_cast<float>(x2), static_cast<float>(y2),
                       static_cast<float>(x3), static_cast<float>(y3)};

    float dstBuff[] = {static_cast<float>(0), static_cast<float>(0),
                       static_cast<float>(bitmapWidth), static_cast<float>(0),
                       static_cast<float>(bitmapWidth), static_cast<float>(bitmapHeight),
                       static_cast<float>(0), static_cast<float>(bitmapHeight)};

    cv::Mat src(4, 1, CV_32FC2, srcBuff);
    cv::Mat dst(4, 1, CV_32FC2, dstBuff);

    cv::Mat perspectiveTransform = cv::getPerspectiveTransform(src, dst);
    cv::warpPerspective(
            sourceImg, destImg, perspectiveTransform, cv::Size(bitmapWidth, bitmapHeight),
            cv::INTER_LINEAR,
            borderMode, borderValue);
}