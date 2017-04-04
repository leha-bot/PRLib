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

#include "Deskew.hpp"

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#if defined(WIN32) || defined(_MSC_VER)
#include <windows.h>
#else
#endif

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable : 4305)
# pragma warning(disable : 4005)
#endif // _MSC_VER

#include <leptonica/allheaders.h>

#ifdef _MSC_VER
# pragma warning(pop)
#endif // _MSC_VER

#if defined(_WIN32) || defined(_MSC_VER)
#include <direct.h>
#include <codecvt>
#define GetCurrentDir _wgetcwd
#define STRLEN wcslen
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#define STRLEN strlen
#endif //WIN32

#include <algorithm>
#include <functional>
#include <cctype>
#include <list>
#include <map>

#include <leptonica/pix.h>


#ifndef M_PI

#ifdef CV_PI
#define M_PI CV_PI
#else
#define M_PI 3.141592653
#endif


#endif // !M_PI


typedef int LONG;
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned short WORD;

struct BITMAPFILEHEADER
{
    ushort bfType;
    uint   bfSize;
    uint   bfReserved;
    uint   bfOffBits;
};

struct BITMAPINFOHEADER
{
    uint  biSize;
    int   biWidth;
    int   biHeight;
    short biPlanes;
    short biBitCount;
    uint  biCompression;
    uint  biSizeImage;
    int   biXPelsPerMeter;
    int   biYPelsPerMeter;
    uint  biClrUsed;
    uint  biClrImportant;
};

struct RGBQUAD
{
    unsigned char rgbBlue;
    unsigned char rgbGreen;
    unsigned char rgbRed;
    unsigned char rgbReserved;
};

struct BITMAPINFO
{
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD          bmiColors[256];
};








int hmod(int num, int denom)
{
    CV_Assert(denom != 0);
    if (num % denom == 0)
    {
        return num / denom;
    }
    else
    {
        return num / denom + 1;
    }
}

#define DIBBYTESPERLINE(bits) (((bits) + 31) / 32 * 4)

static PIX* _CreatePIX(const BITMAPINFO& Bmi2, unsigned char* pBytes)
{
    if (Bmi2.bmiHeader.biCompression != 0 || Bmi2.bmiHeader.biBitCount != 1)
    {
        CV_Assert(Bmi2.bmiHeader.biCompression == 0);
        CV_Assert(Bmi2.bmiHeader.biBitCount == 1);
        return NULL;
    }

    PIX* pix = pixCreate(Bmi2.bmiHeader.biWidth, abs(Bmi2.bmiHeader.biHeight), 1);
    if (!pix)
    {
        return NULL;
    }

    pixSetXRes(pix, (l_int32) ((l_float32) Bmi2.bmiHeader.biXPelsPerMeter / 39.37 + 0.5)); /* to ppi */
    pixSetYRes(pix, (l_int32) ((l_float32) Bmi2.bmiHeader.biYPelsPerMeter / 39.37 + 0.5)); /* to ppi */

    {
        PIXCMAP* cmap = pixcmapCreate(1);
        memcpy(cmap->array, &Bmi2.bmiColors[0], 2 * sizeof(RGBA_QUAD));
        cmap->n = 2;
        pixSetColormap(pix, cmap);
    }

    {
        const int pixBpl = 4 * pixGetWpl(pix);

        unsigned char* pDest = (unsigned char*) pixGetData(pix);
        unsigned char* pSrc = pBytes;
        int biDestInc = pixBpl;
        int biBPLSrc = DIBBYTESPERLINE(Bmi2.bmiHeader.biWidth * 1);
        if (Bmi2.bmiHeader.biHeight > 0)
        {
            pDest = (unsigned char*) pixGetData(pix) + pixBpl * (Bmi2.bmiHeader.biHeight - 1);
            biDestInc = -pixBpl;
        }

        for (int i = 0; i < abs(Bmi2.bmiHeader.biHeight); i++)
        {
            memcpy(pDest, pSrc, biBPLSrc);
            pDest += biDestInc;
            pSrc += biBPLSrc;
        }
    }

    pixEndianByteSwap(pix);

/* —------------------------------------------—
* The bmp colormap determines the values of black
* and white pixels for binary in the following way:
* if black = 1 (255), white = 0
* 255, 255, 255, 0, 0, 0, 0, 0
* if black = 0, white = 1 (255)
* 0, 0, 0, 0, 255, 255, 255, 0
* We have no need for a 1 bpp pix with a colormap!
* —------------------------------------------— */
    {
        PIX* pPixTemp;
        if ((pPixTemp = pixRemoveColormap(pix, REMOVE_CMAP_BASED_ON_SRC)) == NULL)
        {
            pixDestroy(&pix);
            return NULL;
        }
        pixDestroy(&pix);
        pix = pPixTemp;
    }

    return pix;
}


PIX* ImgOpenCvToLepton(const cv::Mat src)
{
    PIX* pix = NULL;

    unsigned char* buffer = new unsigned char[sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD)];
    BITMAPINFO* bmi = (BITMAPINFO*) buffer;

    bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD);
    bmi->bmiHeader.biWidth = src.size().width;
    bmi->bmiHeader.biHeight = src.size().height;
    bmi->bmiHeader.biPlanes = 1;
    bmi->bmiHeader.biBitCount = 1;
    bmi->bmiHeader.biCompression = 0;
    bmi->bmiHeader.biSizeImage = 0;
    bmi->bmiHeader.biXPelsPerMeter = 0;
    bmi->bmiHeader.biYPelsPerMeter = 0;
    bmi->bmiHeader.biClrUsed = 0;
    bmi->bmiHeader.biClrImportant = 0;

    {
        RGBQUAD* colormap = (RGBQUAD * )(buffer + sizeof(BITMAPINFOHEADER));
        colormap[0].rgbBlue = 0xFF;
        colormap[0].rgbGreen = 0xFF;
        colormap[0].rgbRed = 0xFF;
        colormap[1].rgbBlue = 0x00;
        colormap[1].rgbGreen = 0x00;
        colormap[1].rgbRed = 0x00;
    }

    int psize = hmod(src.size().width, 32) * 4 * src.size().height;
    unsigned char* bytes = new unsigned char[psize];
    memset(bytes, 0, psize);
    pix = _CreatePIX(*bmi, bytes);
    delete[] bytes;
    bytes = (unsigned char*)(pix->data);

    unsigned char* blockPointer = &bytes[3];
    int blockDelta = 4;

    for (int j = 0; j < src.size().height; j++)
    {
        for (int i = 0; i < src.size().width; i++)
        {
/*check whether or not we have to move the block pointer*/
            if ((i % 32 == 0) && !((j == 0) && (i == 0)))
            {
                blockPointer += blockDelta;
            }

/* Get value from cv::Mat*/
            const unsigned char cell = src.at<unsigned char>(j, i);
            bool isBlack = false;
            if ((cell != 0) && (cell != 0xff))
            {}
            else if (cell == 0)
            {
                isBlack = true;
            }
            if (isBlack)
            {
/*define value to set*/
                unsigned char value = 0;
                int r = i % 4;
                switch (r)
                {
                    case 0:
                        value += 8;
                        break;
                    case 1:
                        value += 4;
                        break;
                    case 2:
                        value += 2;
                        break;
                    default:
                        value += 1;
                        break;
                }

/*define increasable position*/
                unsigned char* incPos = blockPointer;

                int ii = i % 32;
                switch ((int) (ii / 8))
                {
                    case 1:
                        incPos -= 1;
                        break;
                    case 2:
                        incPos -= 2;
                        break;
                    case 3:
                        incPos -= 3;
                        break;
                    default:
                        break;
                }
/*define increment value*/

                int vv = i % 8;
                if (vv < 4)
                {
                    *incPos += value * 16;
                }
                else
                {
                    *incPos += value;
                }
            }
        }
    }

    return pix;
}

cv::Mat ImgLeptonToOpenCV(const PIX* src)
{
    cv::Size size(src->w, src->h);
    cv::Mat mat = cv::Mat(size, CV_8UC1);
    const unsigned char* pp = (unsigned char*) (src->data);
    const int psize = hmod(src->w, 32) * 4 * src->h;

    l_uint32 i = 0;
    int j = 0;

    int k = 3;

    while (k < psize)
    {
        unsigned char bValue = pp[k];
        unsigned char bHigh = bValue & 0xF0;
        unsigned char bLow = bValue & 0x0F;

        unsigned char divider = 8 * 16;

        while ((i < src->w) && (divider >= 16))
        {
            unsigned char v = bHigh / divider;
            if (v > 0)
            {
                mat.at<unsigned char>(j, i) = 0x00;
            }
            else
            {
                mat.at<unsigned char>(j, i) = 0xff;
            }
            i++;
            bHigh = bHigh - v * divider;
            divider = divider / 2;
        }

        divider = 8;

        while ((i < src->w) && (divider > 0))
        {
            unsigned char v = bLow / divider;
            if (v > 0)
            {
                mat.at<unsigned char>(j, i) = 0x00;
            }
            else
            {
                mat.at<unsigned char>(j, i) = 0xff;
            }
            i++;
            bLow = bLow - v * divider;
            divider = divider / 2;
        }

        if (i >= src->w)
        {
            i = 0;
            j++;

            while (k % 4 != 0)
            {
                k--;
            }
            k += 7;
            continue;
        }

        if (k % 4 == 0)
        {
            k += 7;
        }
        else
        {
            k--;
        }
    }

    return mat;
}

double FindOrientation(const cv::Mat& input)
{
    PIX* pix = ImgOpenCvToLepton(input);
    if (!pix)
    {
        return 0;
    }

    l_int32 iOrientation = 0;
    {
        l_float32 fUpConf;
        l_float32 fLeftConf;
        if (pixOrientDetectDwa(pix, &fUpConf, &fLeftConf, 0, 0) != 0)
        {
            if (pix)
            {
                pixDestroy(&pix);
            }

            return 0;
        }

        if (makeOrientDecision(fUpConf, fLeftConf, 0.0, 0.0, &iOrientation, 0) != 0)
        {
            if (pix)
            {
                pixDestroy(&pix);
            }

            return 0;
        }
    }

    double angle = 0;
    if (iOrientation == L_TEXT_ORIENT_UP)
    {
        angle = 0.0;
    }
    else if (iOrientation == L_TEXT_ORIENT_LEFT)
    {
        angle = 90.0;
    }
    else if (iOrientation == L_TEXT_ORIENT_DOWN)
    {
        angle = 180.0;
    }
    else if (iOrientation == L_TEXT_ORIENT_RIGHT)
    {
        angle = 270.0;
    }
    else
    { // if (iOrientation == L_TEXT_ORIENT_UNKNOWN)
        angle = 0.0;
    }

    pixDestroy(&pix);

    return angle;
}

bool compare_pairs(const std::pair<double, int>& p1, const std::pair<double, int>& p2)
{
    return p1.second < p2.second;
}

bool eq_d(double v1, double v2, double delta)
{
    if (std::abs(v1 - v2) <= delta)
    {
        return true;
    }

    return false;
}

double FindAngle(const cv::Mat& input_orig)
{
    // AA: we need black-&-white image here even if none of threshold algorithms were called before
    //cv::adaptiveThreshold(input, input, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 15, 5);
    cv::Mat input = input_orig.clone();

    cv::Size imgSize = input.size();
    cv::bitwise_not(input, input);
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(input, lines, 1, CV_PI / 180, 100, imgSize.width / 8.f, 20);
    cv::Mat disp_lines(imgSize, CV_8UC1, cv::Scalar(0, 0, 0));

    const size_t nb_lines = static_cast<size_t>(lines.size());
    if (nb_lines == 0)
    {
        return 0;
    }

    std::vector<double> cv_angles = std::vector<double>(nb_lines);

    for (size_t i = 0; i < nb_lines; ++i)
    {
        cv::line(
                disp_lines,
                cv::Point(lines[i][0], lines[i][1]),
                cv::Point(lines[i][2], lines[i][3]),
                cv::Scalar(255, 0, 0));

        cv_angles[i] = atan2(
                (double) lines[i][3] - lines[i][1],
                (double) lines[i][2] - lines[i][0]);
    }

    const double delta = 0.01; //difference is less than 1 deg.
    std::list<std::pair<double, int> > t_diff;

    for (auto it = cv_angles.begin(); it != cv_angles.end(); ++it)
    {
        bool found = false;
        // bypass list
        for (std::list<std::pair<double, int>>::iterator elem = t_diff.begin(); elem != t_diff.end(); ++elem)
        {
            if (eq_d(*it, elem->first, delta))
            {
                elem->second++;
                found = true;
                break;
            }
        }

        if (!found)
        {
            std::pair<double, int> p(*it, 0);
            t_diff.push_back(p);
        }
    }

    std::pair<double, int> max_elem =
            *(std::max_element(t_diff.begin(), t_diff.end(), compare_pairs));

    const double cv_angle = max_elem.first * 180 / M_PI;

    cv::bitwise_not(input, input);

    return cv_angle;
}

cv::Mat Rotate(const cv::Mat& input, const double angle)
{
    if (angle == 90)
    {
        cv::Mat output;

        // rotate on 90
        cv::transpose(input, output);
        cv::flip(output, output, 1);

        return output.clone();
    }
    else if (angle == 180)
    {
        // rotate on 180
        cv::Mat output;
        cv::flip(input, output, -1);

        return output.clone();
    }
    else if (angle == 270)
    {
        cv::Mat output;

        // rotate on 270
        cv::transpose(input, output);
        cv::flip(output, output, 0);

        return output.clone();
    }
    else
    {
        cv::Mat output = input.clone();
        cv::bitwise_not(input, input);

        int len = std::max(output.cols, output.rows);
        cv::Point2f pt(static_cast<float>(len / 2.0), static_cast<float>(len / 2.0));
        cv::Mat r = cv::getRotationMatrix2D(pt, angle, 1.0);

        cv::warpAffine(input, output, r, cv::Size(len, len));

        cv::bitwise_not(input, input);
        cv::bitwise_not(output, output);

        return output.clone();
    }
}

bool prl::deskew(cv::Mat& inputImage, cv::Mat& deskewedImage)
{
    cv::Mat processingImage;

    if (inputImage.channels() != 1)
    {
        cv::cvtColor(inputImage, processingImage, CV_BGR2GRAY);
    }
    else
    {
        processingImage = inputImage.clone();
    }

    cv::threshold(processingImage, processingImage, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

    double angle = FindAngle(processingImage);

    if ((angle != 0) && (angle <= DBL_MAX && angle >= -DBL_MAX))
    {
        deskewedImage = Rotate(inputImage, angle);
    }
    else
    {
        deskewedImage = inputImage.clone();
    }

    angle = FindOrientation(deskewedImage);

    if (angle != 0)
    {
        deskewedImage = Rotate(deskewedImage, angle);
    }

    return !deskewedImage.empty();
}