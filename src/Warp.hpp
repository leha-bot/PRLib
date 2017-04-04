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

#ifndef UFOCR_WARP_HPP
#define UFOCR_WARP_HPP

#include "opencv2/core.hpp"

namespace IPL
{
void warpCrop(const cv::Mat& sourceImg,
              cv::Mat& destImg,
              const int x0, const int y0,
              const int x1, const int y1,
              const int x2, const int y2,
              const int x3, const int y3,
              const int borderMode = cv::BORDER_CONSTANT,
              const cv::Scalar& borderValue = cv::Scalar());
}

#endif //UFOCR_WARP_HPP
