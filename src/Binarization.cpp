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

#include "Binarization.hpp"

#include "SauvolaBinarization.h"

#include "opencv2/imgproc.hpp"
#include "opencv2/ximgproc.hpp"

void prl::binarize(const cv::Mat& src, cv::Mat& dst,
                   BinarizationMethod method /*= BinarizationMethod::Otsu*/)
{
    switch (method)
    {
        case BinarizationMethod::Otsu:
            cv::threshold(src, dst, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
            break;
        case BinarizationMethod::Adaptive:
            cv::adaptiveThreshold(src, dst, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 13, 1);
            break;
        case BinarizationMethod::Niblack:
            //TODO: rewrite constants in Niblack binarization
            cv::ximgproc::niBlackThreshold(src, dst, 255, cv::THRESH_BINARY, 2 * (11) + 1, ((8.0) - 10.0) / 10.0);
            break;
        case BinarizationMethod::Sauvola:
        {
            cv::Mat temp = src.clone();
            prl::sauvolaBinarization(temp, dst);
        }
        default:
            //TODO: Maybe later we should add more binarization algorithms
            throw std::runtime_error("Binarization algorithm not implemented yet!");
    }
}
