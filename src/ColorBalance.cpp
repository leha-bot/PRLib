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