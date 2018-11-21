//
//  qr_image_extractor.cpp
//  QRCode_Mac
//
//  Created by Alexander Graschenkov on 21/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#include "qr_image_extractor.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

using namespace std;
using namespace cv;


cv::Mat extractQR(const cv::Mat &grayImg, std::vector<Dot> dots) {
    if (dots.size() != 4)
        return Mat();
    
    // зная свойства расположения четырехуголника меняем порядок
    swap(dots[2], dots[3]);
    vector<Point2f> srcPoints;
    for (Dot d : dots) {
        srcPoints.push_back(d.pos);
    }
    
    Size outSize(200, 200);
    float offset = 20;
    vector<Point> dstPoints = { Point2f(offset, offset),
                                Point2f(outSize.width - offset, offset),
                                Point2f(outSize.width - offset, outSize.height - offset),
                                Point2f(offset, outSize.width - offset) };
    
    Mat h = findHomography(srcPoints, dstPoints);
    Mat wrapped;
    warpPerspective(grayImg, wrapped, h, outSize);
    
    return wrapped;
}
