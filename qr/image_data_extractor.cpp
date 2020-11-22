//
//  image_data_extractor.cpp
//  QRCode_Mac
//
//  Created by Alexander Graschenkov on 22/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#include "image_data_extractor.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

using namespace cv;
using namespace std;


enum Side {
    SideTop = 0, SideRight = 1, SideBot = 2, SideLeft = 3
};

// Just for debug
void rectangeWithAlpha(cv::Mat &mat, cv::Rect rect, cv::Scalar color, float alpha) {
    rect = rect & Rect(0,0,mat.cols,mat.rows);
    cv::Mat roi = mat(rect);
    cv::Mat colorMat(roi.size(), CV_8UC3, color);
    cv::addWeighted(colorMat, alpha, roi, 1.0 - alpha , 0.0, roi);
}

Point2f applyTransform(Point2f p, Size size, Side side) {
    if (side == SideRight) {
        return Point2f(size.width - p.y, p.x);
    } else if (side == SideBot) {
        return Point2f(size.width - p.x, size.height - p.y);
    } else if (side == SideLeft) {
        return Point2f(p.y, size.height - p.x);
    }
    return p;
}

QRData getQRBitsData(const cv::Mat &grayImg, int rowSize, Mat *debugImg) {
    if (debugImg) {
        cvtColor(grayImg, *debugImg, COLOR_GRAY2BGR);
    }
    
    int startOffset = 43;
    int borderOffset = 4;
    int step = 10;
    int dataWidth = 3;
    int dataSize = grayImg.cols - startOffset * 2;
    
    QRData qr;
    for (int side = 0; side < 4; side++) {
        for (int level = 0; level < 3; level++) {
            qr.group(side).row(2-level).resize(rowSize , false);
            for (int i = 0; i < rowSize; i++) {
                float dataPercent = i / (float)(rowSize-1);
                Point2f center(startOffset + dataPercent * dataSize, level * step + borderOffset);
                center = applyTransform(center, grayImg.size(), (Side)side);
                Rect r(center.x - dataWidth / 3,
                       center.y - dataWidth / 3,
                       2 * dataWidth / 3 + 1,
                       2 * dataWidth / 3 + 1);
                bool isFilled = (mean(grayImg(r))[0] < 100);
                if (debugImg) {
                    rectangeWithAlpha(*debugImg, r, isFilled ? CV_RGB(0, 255, 0) : CV_RGB(255, 0, 0), 0.5);
                }
                
                qr.group(side).row(2-level)[i] = isFilled;
//                Mat temp;
//                pyrUp(debugImg, temp);
//                imshow("Data", temp);
//                waitKey(1);
            }
        }
    }
    
//    waitKey();
    return qr;
}


cv::Mat extractQRArea(const cv::Mat &grayImg, std::vector<Dot> dots) {
    if (dots.size() != 4)
        return Mat();
    
    // зная свойства расположения четырехуголника меняем порядок
    swap(dots[2], dots[3]);
    vector<Point2f> srcPoints;
    for (Dot d : dots) {
        srcPoints.push_back(d.pos);
    }
    
    Size outSize(200, 200);
    float offset = 16;
    vector<Point> dstPoints = { Point2f(offset, offset),
        Point2f(outSize.width - offset, offset),
        Point2f(outSize.width - offset, outSize.height - offset),
        Point2f(offset, outSize.width - offset) };
    
    Mat h = findHomography(srcPoints, dstPoints);
    Mat wrapped;
    warpPerspective(grayImg, wrapped, h, outSize);
    
    return wrapped;
}
