//
//  qr_data_coder.cpp
//  QRCode_Mac
//
//  Created by Alexander Graschenkov on 24/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#include "qr_data_coder.hpp"
#include <math.h>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "image_data_extractor.hpp"

using namespace std;
using namespace cv;

Group& QRData::group(int n) {
    if (n == 0) {
        return g1;
    } else if (n == 1) {
        return g2;
    } else if (n == 2) {
        return g3;
    } else {
        return g4;
    }
}
Group QRData::group(int n) const {
    if (n == 0) {
        return g1;
    } else if (n == 1) {
        return g2;
    } else if (n == 2) {
        return g3;
    } else {
        return g4;
    }
}

RowData& Group::row(int n) {
    if (n == 0) {
        return r1;
    } else if (n == 1) {
        return r2;
    } else {
        return r3;
    }
}

RowData Group::row(int n) const {
    if (n == 0) {
        return r1;
    } else if (n == 1) {
        return r2;
    } else {
        return r3;
    }
}


int indexQRData(QRData &data, int idx) {
    int idx2 = idx - 1;
    if (idx2 < 0) idx2 += 4;
    
    bool vals[] = {false, false, false};
    for (int i = 0; i < 3; i++) {
        vals[i] = data.group(idx).row(i).front() || data.group(idx2).row(i).back();
    }
    if (vals[0] && vals[1] && vals[2]) {
        return 0;
    }
    for (int i = 0; i < 3; i++) {
        if (vals[i]) return i+1;
    }
    return -1;
}

void mirrorQRData(QRData &data) {
    swap(data.g1, data.g4);
    swap(data.g2, data.g3);
    for (int groupIdx = 0; groupIdx < 4; groupIdx++) {
        for (int level = 0; level < 3; level++) {
            auto &row = data.group(groupIdx).row(level);
            reverse(row.begin(), row.end());
        }
    }
}

QRData orderQRData(QRData data) {
    // первый блок
    vector<int> order;
    for (int i = 0; i < 4; i++) {
        int idx = indexQRData(data, i);
        order.push_back(idx);
    }
    bool isNormal = true; // not mirrored
    for (int i = 0; i < 4; i++) {
        int ii = (i + 1) % 4;
        isNormal = isNormal && (order[i] < order[ii] || (order[i] - order[ii]) > 2);
    }
    
    if (!isNormal) {
        mirrorQRData(data);
        order.clear();
        for (int i = 0; i < 4; i++) {
            int idx = indexQRData(data, i);
            order.push_back(idx);
        }
    }
    while (order.front() != 0) {
        order.erase(order.begin());
        // как метод сортировки пузырьком
        swap(data.g1, data.g2);
        swap(data.g2, data.g3);
        swap(data.g3, data.g4);
    }
    return data;
}


QRDataCoder::QRDataCoder(int messageSize, double correctPercentSize):
    alphabet(Alphabet(messageSize, correctPercentSize)) {
    
}


std::string QRDataCoder::decode(QRData &data) {
    QRData ordered = orderQRData(data);
    // удаляем биты для правильной сортировки
    for (int i = 0; i < 4; i++) {
        for (int rowIdx = 0; rowIdx < 3; rowIdx++) {
            auto &r = ordered.group(i).row(rowIdx);
            r.pop_back();
            r.erase(r.begin());
        }
    }
    
    vector<bool> bits;
    for (int groupIdx = 0; groupIdx < 4; groupIdx++) {
        for (int rowIdx = 0; rowIdx < 3; rowIdx++) {
            auto &r = ordered.group(groupIdx).row(rowIdx);
            bits.insert(bits.end(), r.begin(), r.end());
        }
    }
    while (bits.size() > alphabet.totalMessageSizeBits()) {
        bits.pop_back();
    }
    string str = alphabet.decode(bits);
    return str;
}

QRData QRDataCoder::encode(std::string message) {
    vector<bool> bits = alphabet.encode(message);
    int rowsCount = 3*4;
    int resizeToBits = ceil(bits.size() / (double)rowsCount) * rowsCount;
    while (resizeToBits > bits.size()) {
        bits.push_back(false);
    }
    
    // заполняем все данными
    QRData data;
    int bitsOffset = 0;
    int bitsStep = (int)bits.size() / rowsCount;
    for (int groupIdx = 0; groupIdx < 4; groupIdx++) {
        for (int rowIdx = 0; rowIdx < 3; rowIdx++) {
            data.group(groupIdx).row(rowIdx) = vector<bool>(bits.begin() + bitsOffset,
                                                            bits.begin() + bitsOffset + bitsStep);
            bitsOffset += bitsStep;
        }
    }
    
    // добавляем дополнительные данные для сохранения порядка
    // добавляем с двух сторон
    // 0 - true true true
    // 1 - true false false
    // 2 - false true false
    // 3 - false false true
    for (int i = 0; i < 4; i++) {
        int ii = (i + 1) % 4;
        for (int rowIdx = 0; rowIdx < 3; rowIdx++) {
            bool isFill = (ii == 0 || i == rowIdx);
            
            data.group(i).row(rowIdx).push_back(isFill);
            auto &r = data.group(ii).row(rowIdx);
            r.insert(r.begin(), isFill);
        }
    }
    return data;
}

int QRDataCoder::getRowSize() const {
    int rowsCount = 3*4;
    int bitsCount = alphabet.totalMessageSizeBits();
    return ceil(bitsCount / (double)rowsCount) + 2;
}

std::string QRDataCoder::processQRCode(const cv::Mat &colorImg, cv::Mat *warpedQRImage) {
    if (colorImg.channels() == 1) {
        threshold(colorImg, grayImg, 110, 255, CV_THRESH_BINARY);
    } else {
        cvtColor(colorImg, grayImg, CV_BGR2GRAY);
        threshold(grayImg, grayImg, 110, 255, CV_THRESH_BINARY);
    }
    
    vector<Dot> foundedDots = findDots(grayImg);
    
//    Mat debugImg;
//    cvtColor(grayImg, debugImg, CV_GRAY2BGR);
//    for (Dot p : foundedDots) {
//        circle(debugImg, p.pos, 3, CV_RGB(0, 255, 0), CV_FILLED);
//        rectangle(debugImg, p.area, CV_RGB(0, 255, 0), 2);
//    }
    Mat extracted = extractQRArea(grayImg, foundedDots);
    if (extracted.cols > 0) {
        QRDataCoder coder;
        QRData data = getQRBitsData(extracted, coder.getRowSize(), warpedQRImage);
        string str = coder.decode(data);
        return str;
    }
    
    return "";
}

