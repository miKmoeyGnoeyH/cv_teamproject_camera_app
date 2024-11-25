#pragma once
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

void removePepperNoise(Mat& mask);

Mat auto_brightness_adjust(Mat src) {
    Mat labImage;
    cvtColor(src, labImage, COLOR_BGR2Lab);
    vector<Mat> labChannels;
    split(labImage, labChannels);

    Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8, 8));
    clahe->apply(labChannels[0], labChannels[0]);

    merge(labChannels, labImage);
    cvtColor(labImage, src, COLOR_Lab2BGR);

    Mat blurredImage;
    GaussianBlur(src, blurredImage, Size(5, 5), 1.0);
    Mat sharpImage = src + (src - blurredImage);

    Mat finalImage;
    bilateralFilter(sharpImage, finalImage, 15, 80, 80);

    return finalImage;
}

Mat filter_gray(Mat src) {
    Mat dst;
    cvtColor(src, dst, COLOR_BGR2GRAY);
    return dst;
}

Mat filter_reverse(Mat src) {
    Mat dst = Mat::zeros(src.size(), src.type());
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            Vec3b pixel = src.at<Vec3b>(y, x);
            dst.at<Vec3b>(y, x) = Vec3b(255, 255, 255) - pixel;
        }
    }
    return dst;
}

Mat filter_warm(Mat src) {
    Mat dst;

    vector<Mat> channels(3);
    split(src, channels);

    channels[2] = channels[2] + 20;
    channels[1] = channels[1] + 10;
    channels[0] = channels[0] - 10;

    for (int i = 0; i < 3; i++) {
        threshold(channels[i], channels[i], 255, 255, THRESH_TRUNC);
        threshold(channels[i], channels[i], 0, 0, THRESH_TOZERO);
    }

    merge(channels, dst);

    dst.convertTo(dst, -1, 1.1, 10);

    return dst;
}

Mat filter_cool(Mat src) {
    Mat dst;
    vector<Mat> channels(3);
    split(src, channels);

    channels[2] = channels[2] - 10;
    channels[0] = channels[0] + 20;

    for (int i = 0; i < 3; i++) {
        threshold(channels[i], channels[i], 255, 255, THRESH_TRUNC);
        threshold(channels[i], channels[i], 0, 0, THRESH_TOZERO);
    }

    merge(channels, dst);

    dst.convertTo(dst, -1, 1.1, -10);

    return dst;
}

Mat filter_cartoon(Mat src) {
    Mat gray, edges, color, cartoon;

    cvtColor(src, gray, COLOR_BGR2GRAY);

    medianBlur(gray, gray, 7);
    adaptiveThreshold(gray, edges, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 9, 2);

    Mat img_bilateral;
    bilateralFilter(src, img_bilateral, 9, 75, 75);

    cvtColor(edges, edges, COLOR_GRAY2BGR);
    cartoon = img_bilateral & edges;

    return cartoon;
}


Mat cartoonifyImage(Mat src) {
    Mat src_temp, gray, edges, mask;
    Mat dst;
    src_temp = src.clone();
    Size size = src_temp.size();

    cvtColor(src_temp, gray, COLOR_BGR2GRAY);

    const int MEDIAN_BLUR_FILTER_SIZE = 7;
    medianBlur(gray, gray, MEDIAN_BLUR_FILTER_SIZE);

    const int LAPLACIAN_FILTER_SIZE = 5;
    Laplacian(gray, edges, CV_8U, LAPLACIAN_FILTER_SIZE);

    const int EDGES_THRESHOLD = 80;
    threshold(edges, mask, EDGES_THRESHOLD, 255, THRESH_BINARY_INV);

    removePepperNoise(mask);

    Size smallSize;
    smallSize.width = size.width / 2;
    smallSize.height = size.height / 2;
    Mat smallImg = Mat(smallSize, CV_8UC3);
    resize(src_temp, smallImg, smallSize, 0, 0, INTER_LINEAR);

    Mat tmp = Mat(smallSize, CV_8UC3);
    int repetitions = 7;
    for (int i = 0; i < repetitions; i++)
    {
        int size = 9;
        double sigmaColor = 9;
        double sigmaSpace = 7;
        bilateralFilter(smallImg, tmp, size, sigmaColor, sigmaSpace);
        bilateralFilter(tmp, smallImg, size, sigmaColor, sigmaSpace);
    }

    resize(smallImg, src_temp, size, 0, 0, INTER_LINEAR);
    dst.setTo(0);
    src_temp.copyTo(dst, mask);

    return dst;
}


void removePepperNoise(Mat& mask)
{
    for (int y = 2; y < mask.rows - 2; y++) {
        uchar* pThis = mask.ptr(y);
        uchar* pUp1 = mask.ptr(y - 1);
        uchar* pUp2 = mask.ptr(y - 2);
        uchar* pDown1 = mask.ptr(y + 1);
        uchar* pDown2 = mask.ptr(y + 2);

        pThis += 2;
        pUp1 += 2;
        pUp2 += 2;
        pDown1 += 2;
        pDown2 += 2;
        for (int x = 2; x < mask.cols - 2; x++) {
            uchar v = *pThis;
            if (v == 0) {
                bool allAbove = *(pUp2 - 2) && *(pUp2 - 1) && *(pUp2) && *(pUp2 + 1) && *(pUp2 + 2);
                bool allLeft = *(pUp1 - 2) && *(pThis - 2) && *(pDown1 - 2);
                bool allBelow = *(pDown2 - 2) && *(pDown2 - 1) && *(pDown2) && *(pDown2 + 1) && *(pDown2 + 2);
                bool allRight = *(pUp1 + 2) && *(pThis + 2) && *(pDown1 + 2);
                bool surroundings = allAbove && allLeft && allBelow && allRight;
                if (surroundings == true) {
                    *(pUp1 - 1) = 255;
                    *(pUp1 + 0) = 255;
                    *(pUp1 + 1) = 255;
                    *(pThis - 1) = 255;
                    *(pThis + 0) = 255;
                    *(pThis + 1) = 255;
                    *(pDown1 - 1) = 255;
                    *(pDown1 + 0) = 255;
                    *(pDown1 + 1) = 255;
                }
                pThis += 2;
                pUp1 += 2;
                pUp2 += 2;
                pDown1 += 2;
                pDown2 += 2;
            }
            pThis++;
            pUp1++;
            pUp2++;
            pDown1++;
            pDown2++;
        }
    }
}
