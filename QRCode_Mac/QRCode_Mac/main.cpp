//
//  main.cpp
//  QRCode_Mac
//
//  Created by Alexander Graschenkov on 20/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "dot_detector.hpp"

using namespace std;
using namespace cv;

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    Mat img = imread("qr_image.jpg");
    Mat grayImg;
    cvtColor(img, grayImg, CV_BGR2GRAY);
    threshold(grayImg, grayImg, 110, 255, CV_THRESH_BINARY);
    vector<Dot> foundedPoints = findDots(grayImg);
    
    Mat debugImg;
    cvtColor(grayImg, debugImg, CV_GRAY2BGR);
    for (Dot p : foundedPoints) {
        circle(debugImg, p.pos, 3, CV_RGB(0, 255, 0), CV_FILLED);
        rectangle(debugImg, p.area, CV_RGB(0, 255, 0), 2);
    }
    
    imshow("Source", img);
    imshow("Founded points", debugImg);
    waitKey();
    
    return 0;
}
