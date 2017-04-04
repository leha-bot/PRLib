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

#include "Cropping.hpp"

#include "opencv2/imgproc.hpp"

bool isQuadrangle(const std::vector<cv::Point>& contour)
{
    return contour.size() == 4;
}

std::vector<cv::Point> orderPoints(const std::vector<cv::Point>& contour)
{
    //http://www.pyimagesearch.com/2014/08/25/4-point-opencv-getperspective-transform-example/
    std::vector<std::pair<int, size_t>> sum(contour.size()), diff(contour.size());
    for (size_t i = 0; i < contour.size(); ++i)
    {
        sum[i] = {contour[i].x + contour[i].y, i};
        diff[i] = {contour[i].x - contour[i].y, i};
    }
    std::sort(sum.begin(), sum.end());
    std::sort(diff.begin(), diff.end());

    cv::Point topLeft       = contour[sum[0].second],
              topRight      = contour[diff[3].second],
              bottomRight   = contour[sum[3].second],
              bottomLeft    = contour[diff[0].second];

    return {topLeft, topRight, bottomRight, bottomLeft};
}

std::vector<cv::Point> IPL::getContour(const cv::Mat& src, size_t longSide /*= 1024*/)
{
    //TODO: Add more algorithms for border detecting
    //TODO: Research good constants for Canny edge detector
    int thresh = 30;

    //TODO: Add longside support
    cv::Mat src_gray;
    cv::cvtColor(src, src_gray, CV_BGR2GRAY);
    cv::GaussianBlur(src_gray, src_gray, cv::Size(9, 9), 0);

    cv::Mat canny_output;

    /// Detect edges using canny
    cv::Canny(src_gray, canny_output, 25, 50);
    cv::dilate(canny_output, src_gray, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2)),
               cv::Point(-1, -1), 2);
    /// Find contours
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    findContours(canny_output, contours, hierarchy, CV_RETR_CCOMP,
                 CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    std::vector<std::vector<cv::Point>> contoursApproxed(contours.size());
    std::vector<cv::Point> result;
    double maxArea = 0.0;
    for (size_t i = 0; i < contours.size(); ++i)
    {
        cv::approxPolyDP(cv::Mat(contours[i]), contoursApproxed[i], contours[i].size() * 0.2, true);

        if (!isQuadrangle(contoursApproxed[i]))
        {
            continue;
        }

        double area = cv::contourArea(contoursApproxed[i]);
        if (area > maxArea)
        {
            maxArea = area;
            result = contoursApproxed[i];
        }
    }
    result = orderPoints(result);
    return result;
}