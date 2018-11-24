//
//  process_qr.cpp
//  QRCode_iOS
//
//  Created by Alexander Graschenkov on 24/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#include "process_qr.hpp"
#include <opencv2/imgproc.hpp>
#include "dot_detector.hpp"
#include "qr_image_extractor.hpp"
#include "image_data_extractor.hpp"


using namespace std;
using namespace cv;

std::string processQRCode(const cv::Mat &colorImg, cv::Mat *warpedQRImage) {
    Mat grayImg;
    cvtColor(colorImg, grayImg, CV_BGR2GRAY);
    threshold(grayImg, grayImg, 110, 255, CV_THRESH_BINARY);
    vector<Dot> foundedDots = findDots(grayImg);
    
    Mat debugImg;
    cvtColor(grayImg, debugImg, CV_GRAY2BGR);
    for (Dot p : foundedDots) {
        circle(debugImg, p.pos, 3, CV_RGB(0, 255, 0), CV_FILLED);
        rectangle(debugImg, p.area, CV_RGB(0, 255, 0), 2);
    }
    Mat extracted = extractQR(grayImg, foundedDots);
    if (extracted.cols > 0) {
//        if (warpedQRImage) {
//            extracted.copyTo(*warpedQRImage);
//        }
        QRDataCoder coder;
        QRData data = getQRBitsData(extracted, coder.getRowSize(), warpedQRImage);
        string str = coder.decode(data);
        return str;
    }
    
    return "";
}
