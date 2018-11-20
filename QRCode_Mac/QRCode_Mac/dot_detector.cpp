//
//  dot_detector.cpp
//  QRCode_Mac
//
//  Created by Alexander Graschenkov on 21/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#include "dot_detector.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

// ч-черный, б-белый
// Ищем шаблон: 1ч 1б 2.5ч 1б 1ч
bool checkPattern(const int aPattern[5], int maxElemSize = 0, int minElemSize = 0) {
    double matchPattern[5] = {1, 1, 2.5, 1, 1};
    int pattern[5] = {0, 0, 0, 0, 0};
    // нормируем паттерн по предполагаемому масштабу
    for (int i = 0; i < 5; i++) {
        pattern[i] = aPattern[i] / matchPattern[i];
    }
    
    if (maxElemSize > 0) {
        for (int i = 0; i < 5; i++) {
            if (pattern[i] > maxElemSize || pattern[i] < minElemSize)
                return false;
        }
    }
    
    int total = 0;
    for (int i = 0; i < 5; i++) {
        if (pattern[i] == 0)
            return false;
        
        total += pattern[i];
    }
    
    int avgSize = total / 5;
    int maxVariance = avgSize * 0.2;
    
    for (int i = 0; i < 5; i++) {
        if (abs(avgSize - pattern[i]) > maxVariance)
            return false;
    }
    
    return true;
}

inline void shiftPattern(int pattern[5]) {
    pattern[0] = pattern[2];
    pattern[1] = pattern[3];
    pattern[2] = pattern[4];
    pattern[3] = 0;
    pattern[4] = 0;
}

std::vector<cv::Point2f> findDots(const cv::Mat &grayImg) {
    uchar threshVal = 100;
    vector<Point2f> result;
    
    // ищем определенную последовательность чбчбч пикселей
    for (int r = 0; r < grayImg.rows; r++) {
        int sequence[] = {0, 0, 0, 0, 0};
        //                ч, б, ч, б, ч
        // ч - черный, б - белый
        
        bool isWhite = grayImg.at<uchar>(r, 0) > threshVal;
        int idx = isWhite ? 1 : 0;
        
        for (int c = 0; c < grayImg.cols; c++) {
            bool isWhite = grayImg.at<uchar>(r, c) > threshVal;
            bool idxIsWhite = (idx % 2);
            if (isWhite != idxIsWhite) {
                // смена цвета
                idx++;
                if (idx == 5) {
                    // когда весь шаблон заполнен, проверяем его
                    if (checkPattern(sequence)) {
                        int offset = sequence[4] + sequence[3] + ceil(sequence[2] / 2) + 1;
                        
                        Point2f center(c - offset, r);
                        result.push_back(center);
                    }
                    idx -= 2;
                    shiftPattern(sequence);
                }
            }
            sequence[idx]++;
        }
    }
    return result;
}
