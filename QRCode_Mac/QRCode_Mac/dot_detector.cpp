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

static double _matchPattern[5] = {1, 1, 2.5, 1, 1};

// ч-черный, б-белый
// Ищем шаблон: 1ч 1б 2.5ч 1б 1ч
bool checkPattern(const int aPattern[5], int maxElemSize = 0, int minElemSize = 0) {
    int pattern[5] = {0, 0, 0, 0, 0};
    // нормируем паттерн по предполагаемому масштабу
    for (int i = 0; i < 5; i++) {
        pattern[i] = aPattern[i] / _matchPattern[i];
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
    float maxVariance = avgSize * 0.4;
    
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

bool checkOneColumnMat(Mat const &mat, int &index, int &seqSize) {
    int elemSize = seqSize / (_matchPattern[0] +
                              _matchPattern[1] +
                              _matchPattern[2] +
                              _matchPattern[3] +
                              _matchPattern[4]);
    
    int sequence[] = {0, 0, 0, 0, 0};
    int idx = 2;
    
    // для начала заполняем шаблон
    // пробегаемся вниз и вверх по матрице, смотрим цвета
    for (int r = index; r < mat.rows; r++) {
        bool isWhite = mat.at<uchar>(r) > 100;
        bool isWhiteIdx = idx % 2;
        if (isWhite != isWhiteIdx) {
            idx++;
            if (idx == 5) {
                break;
            }
        }
        sequence[idx]++;
    }
    
    int startSequenceRow = -1;
    idx = 2;
    for (int r = index-1; r >= 0; r--) {
        bool isWhite = mat.at<uchar>(r) > 100;
        bool isWhiteIdx = idx % 2;
        if (isWhite != isWhiteIdx) {
            idx--;
            if (idx == -1) {
                startSequenceRow = r;
                break;
            }
        }
        sequence[idx]++;
    }
    
    // и проверка с ограничениями по размерам
    int maxElemSize = elemSize * 2.5;
    int minElemSize = elemSize / 2.5;
    if (checkPattern(sequence, maxElemSize, minElemSize)) {
        index = startSequenceRow + sequence[0] + sequence[1] + floor(sequence[2] / 2.0) + 1;
        seqSize = sequence[0] + sequence[1] + sequence[2] + sequence[3] + sequence[4];
        return true;
    }
    return false;
}

// проверяем кандидата на шаблон по вертикали и диагоналям
// попутно уточняем центр шаблона
void checkAndAddCandidate(const cv::Mat &mat, cv::Point center, int sequence[5], vector<Dot> &dots) {
    for (Dot &d : dots) {
        if (d.area.contains(center)) return;
    }
    
    bool isOk = true;
    int seqSize = sequence[0] + sequence[1] + sequence[2] + sequence[3] + sequence[4];
    
    int idx = 0;
    int height = seqSize;
    Mat vertMat = getOneColumnMat(mat, center.y, center.x, OneRowTypeVertical, idx);
    isOk = isOk && checkOneColumnMat(vertMat, idx, height);
    center.y = idx;
    
    idx = 0;
    int width = seqSize;
    Mat horisMat = getOneColumnMat(mat, center.y, center.x, OneRowTypeHorisontal, idx);
    isOk = isOk && checkOneColumnMat(horisMat, idx, width);
    center.x = idx;
    
    
    idx = 0;
    Mat diagMat = getOneColumnMat(mat, center.y, center.x, OneRowTypeDiagonal1, idx);
    isOk = isOk && checkOneColumnMat(diagMat, idx, seqSize);
    
    idx = 0;
    Mat diagMat2 = getOneColumnMat(mat, center.y, center.x, OneRowTypeDiagonal2, idx);
    isOk = isOk && checkOneColumnMat(diagMat2, idx, seqSize);
    
    if (isOk) {
        Dot newDot(center, Rect(center.x - width / 2, center.y - height / 2, width, height));
        dots.push_back(newDot);
    }
}

// создаем матрицу из одной колонки для удобной сверки шаблона
Mat const getOneColumnMat(Mat const &mat, int row, int column, OneRowType type, int &rowIdxOut) {
    switch (type) {
        case OneRowTypeVertical:
            rowIdxOut = row;
            return mat.col(column);
            break;
            
        case OneRowTypeHorisontal: {
            rowIdxOut = column;
            uchar const *data = mat.ptr(row, 0);
            return Mat(mat.cols, 1, mat.type(), (void*)data, mat.elemSize());
            break;
        }
            
        case OneRowTypeDiagonal1: {
            int before = min(row, column);
            int after = min(mat.rows - row, mat.cols - column);
            rowIdxOut = before;
            
            uchar const *data = mat.ptr(row - before, column - before);
            int step = (int)(mat.step1() + mat.elemSize());
            return Mat(before + after, 1, mat.type(), (void*)data, step);
            break;
        }
            
        case OneRowTypeDiagonal2: {
            int before = min(row, mat.cols - column - 1);
            int after = min(mat.rows - row, column + 1);
            rowIdxOut = before;
            
            uchar const *data = mat.ptr(row - before, column + before);
            int step = (int)(mat.step1() - mat.elemSize());
            return Mat(before + after, 1, mat.type(), (void*)data, step);
            break;
        }
            
    }
    return Mat();
}

std::vector<Dot> findDots(const cv::Mat &grayImg) {
    uchar threshVal = 100;
    vector<Dot> result;
    
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
                        checkAndAddCandidate(grayImg, center, sequence, result);
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
