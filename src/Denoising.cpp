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

#include "Denoising.hpp"

#include "opencv2/xphoto.hpp"
#include "opencv2/photo.hpp"

void IPL::denoise(const cv::Mat& src, cv::Mat& dst)
{
    //Too slow
    //cv::xphoto::dctDenoising(src, dst, 15.0);

    cv::fastNlMeansDenoisingColored(src, dst, 5.5);
}