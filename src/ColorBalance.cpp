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

#include "ColorBalance.hpp"

#include "opencv2/xphoto.hpp"

void prl::whiteBalance(const cv::Mat& src, cv::Mat& dst, ColorBalance method/* = ColorBalance::Simple*/)
{
    switch (method)
    {
        case ColorBalance::Simple:
            cv::xphoto::createSimpleWB()->balanceWhite(src, dst);
            break;
        case ColorBalance::Grayworld:
            cv::xphoto::createGrayworldWB()->balanceWhite(src, dst);
            break;
        default:
            //TODO: Add more algortihms later
            throw std::runtime_error("Balance algorithm is not implemented yet!");
    }

}