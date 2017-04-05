/*
    MIT License

    Copyright (c) 2017 Alexander Zaitsev

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
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