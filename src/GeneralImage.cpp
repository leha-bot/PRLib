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

#include "GeneralImage.hpp"
#include "cvmatandqimage.h"

//TODO: Be careful with internal cv::Mat type

GeneralImage::GeneralImage(const cv::Mat& val) : img_(val.clone())
{
}

GeneralImage::GeneralImage(const QImage& val)
{
    img_ = QtOcv::image2Mat(val, CV_8UC3);
}

QImage GeneralImage::toQImage(QImage::Format format /*= QImage::Format_ARGB32*/) const
{
    return QtOcv::mat2Image(img_);
}

cv::Mat& GeneralImage::Ref()
{
    return img_;
}

cv::Mat GeneralImage::toMat() const
{
    return img_.clone();
}
