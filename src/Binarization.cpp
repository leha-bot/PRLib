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

#include "Binarization.hpp"

#include "opencv2/imgproc.hpp"
#include "opencv2/ximgproc.hpp"

void IPL::binarize(const cv::Mat& src, cv::Mat& dst,
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
        default:
            //TODO: Maybe later we should add more binarization algorithms
            throw std::runtime_error("Binarization algorithm not implemented yet!");
    }
}
