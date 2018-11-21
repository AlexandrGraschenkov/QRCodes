//
//  data_extractor.cpp
//  QRCode_Mac
//
//  Created by Alexander Graschenkov on 22/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#include "data_extractor.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

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

QRData getQRData(const cv::Mat &grayImg, int rowSize) {
    Mat debugImg;
    cvtColor(grayImg, debugImg, CV_GRAY2BGR);
    
    int startOffset = 40;
    int borderOffset = 4;
    int step = 11;
    int dataWidth = 5;
    int dataSize = grayImg.cols - startOffset * 2;
    
    
    for (int side = 0; side < 4; side++) {
        for (int level = 0; level < 3; level++) {
            for (int i = 0; i < rowSize; i++) {
                float dataPercent = i / (float)(rowSize-1);
                Point2f center(startOffset + dataPercent * dataSize, level * step + borderOffset);
                center = applyTransform(center, grayImg.size(), (Side)side);
                Rect r(center.x - dataWidth / 3,
                       center.y - dataWidth / 3,
                       2 * dataWidth / 3 + 1,
                       2 * dataWidth / 3 + 1);
                bool isFilled = (mean(grayImg(r))[0] < 100);
                rectangeWithAlpha(debugImg, r, isFilled ? CV_RGB(0, 255, 0) : CV_RGB(255, 0, 0), 0.5);
                
                Mat temp;
                pyrUp(debugImg, temp);
                imshow("Data", temp);
                waitKey(1);
            }
        }
    }
    
    waitKey();
    return QRData();
}
