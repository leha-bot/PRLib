/*
   PRLib - Pre-Recognition Library
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

#include "SauvolaBinarization.h"

#include "opencv2/imgproc.hpp"

void sauvolaBinarization(
        cv::Mat &imageCopy, cv::Mat &imageSauvola,
        int windowSize /*= 101*/,
        double thresholdCoefficient /*= 0.01*/,
        int morphIterationCount /*= 2*/)
{
    if (imageCopy.channels() != 1)
    {
        cv::cvtColor(imageCopy, imageCopy, CV_BGR2GRAY);
    }

    const int usedFloatType = CV_64FC1;

    //! parameters and constants of algorithm
    int w = std::min(windowSize, std::min(imageCopy.cols, imageCopy.rows));
    int wSqr = w * w;
    double wSqrBack = 1.0 / static_cast<double>(wSqr);
    const double k = thresholdCoefficient;
    const double R = 128;
    const double RBack = 1.0 / R;

    //! add borders
    cv::copyMakeBorder(imageCopy, imageCopy, w / 2, w / 2, w / 2, w / 2, cv::BORDER_REPLICATE);
    cv::Rect processingRect(w / 2, w / 2, imageCopy.cols - w, imageCopy.rows - w);

    cv::Mat integralImage;
    cv::Mat integralImageSqr;

    //! get integral image, ...
    cv::integral(imageCopy, integralImage, integralImageSqr, usedFloatType);
    //! ... crop it and ...
    integralImage = integralImage(cv::Rect(1, 1, integralImage.cols - 1, integralImage.rows - 1));
    //! get square
    integralImageSqr =
            integralImageSqr(cv::Rect(1, 1, integralImageSqr.cols - 1, integralImageSqr.rows - 1));

    //! create storage for local means
    cv::Mat localMeanValues(integralImage.size() - processingRect.size(), usedFloatType);

    //! create filter for local means calculation
    cv::Mat localMeanFilterKernel = cv::Mat::zeros(w, w, usedFloatType);
    localMeanFilterKernel.at<double>(0, 0) = wSqrBack;
    localMeanFilterKernel.at<double>(w - 1, 0) = -wSqrBack;
    localMeanFilterKernel.at<double>(w - 1, w - 1) = wSqrBack;
    localMeanFilterKernel.at<double>(0, w - 1) = -wSqrBack;
    //! get local means
    cv::filter2D(integralImage(processingRect), localMeanValues, usedFloatType,
                 localMeanFilterKernel, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT);

    //! create storage for local deviations
    cv::Mat localMeanValuesSqr = localMeanValues.mul(localMeanValues); // -V678

    //! create filter for local deviations calculation
    cv::Mat localWeightedSumsFilterKernel = localMeanFilterKernel;

    cv::Mat localDevianceValues;

    //! get local deviations
    filter2D(integralImageSqr(processingRect), localDevianceValues, usedFloatType,
             localWeightedSumsFilterKernel);

    localDevianceValues -= localMeanValuesSqr;
    cv::sqrt(localDevianceValues, localDevianceValues);

    //! calculate Sauvola thresholds
    localDevianceValues.convertTo(
            localDevianceValues, localDevianceValues.type(),
            (k * RBack), (1.0 - k));
    cv::Mat thresholdsValues = localMeanValues.mul(localDevianceValues);
    thresholdsValues.convertTo(thresholdsValues, CV_8UC1);

    //! get binarized image
    imageSauvola = imageCopy(processingRect) > thresholdsValues;

    //! apply morphology operation if them required
    if (morphIterationCount > 0)
    {
        cv::dilate(imageSauvola, imageSauvola, cv::Mat(), cv::Point(-1, -1), morphIterationCount);
        cv::erode(imageSauvola, imageSauvola, cv::Mat(), cv::Point(-1, -1), morphIterationCount);
    }
    else
    {
        if (morphIterationCount < 0)
        {
            cv::erode(imageSauvola, imageSauvola, cv::Mat(), cv::Point(-1, -1), -morphIterationCount);
            cv::dilate(imageSauvola, imageSauvola, cv::Mat(), cv::Point(-1, -1), -morphIterationCount);
        }
    }
}