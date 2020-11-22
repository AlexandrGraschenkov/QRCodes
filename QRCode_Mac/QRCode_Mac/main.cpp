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
#include "image_data_extractor.hpp"
#include "alphabet.hpp"

using namespace std;
using namespace cv;

void testMessageCorrection() {
    cout << "---- Run Test Damage Data ----\n";
    int damageCount = 4;
    cout << "Damage bits count " << damageCount << endl << endl;
    vector<string> messages = {"Hello", "world", "we", "can", "any", "message that we want"};
    Alphabet alphabet(30);
    for (string message : messages) {
        vector<bool> bits = alphabet.encode(message);
        // повреждаем данные
        for (int i = 0; i < damageCount; i++) {
            int idx = arc4random() % bits.size();
            bits[idx] = !bits[idx];
        }
        string encodedMessage = alphabet.decode(bits);
        cout << "\"" << message << "\" => \"" << encodedMessage << "\" ";
        
        if (message == encodedMessage) {
            cout << "Success" << endl;
        } else {
            cout << "Error" << endl;
        }
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n\n";
//    testMessageCorrection();
//    return 0;
    
    string imgPath = "/Users/alex/Desktop/qr_example.png";
    if (argc == 2) {
        imgPath = string(argv[1]);
    }
    Mat img = imread(imgPath);
    
    bool debug = false;
    string res = "";
    
    if (!debug) {
        // simple usage
        QRDataCoder coder;
        res = coder.processQRCode(img);
        cout << res << endl;
    } else {
        // for explore algorithm
        Mat grayImg;
        cvtColor(img, grayImg, COLOR_BGR2GRAY);
        threshold(grayImg, grayImg, 110, 255, THRESH_BINARY);
        vector<Dot> foundedDots = findDots(grayImg);
        
        Mat debugImg;
        cvtColor(grayImg, debugImg, COLOR_GRAY2BGR);
        for (Dot p : foundedDots) {
            circle(debugImg, p.pos, 3, CV_RGB(0, 255, 0), FILLED);
            rectangle(debugImg, p.area, CV_RGB(0, 255, 0), 2);
        }
        Mat extracted = extractQRArea(grayImg, foundedDots);
        if (extracted.cols > 0) {
            QRDataCoder coder;
            QRData data = getQRBitsData(extracted, 23);
            string str = coder.decode(data);
            cout << str << endl;
        }
        
        imshow("Source", img);
        imshow("Founded points", debugImg);
        imshow("QR Warped", extracted);
        waitKey();
    }
    
    return 0;
}
