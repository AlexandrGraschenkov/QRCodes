//
//  qr_data_coder.hpp
//  QRCode_Mac
//
//  Created by Alexander Graschenkov on 24/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#ifndef qr_data_coder_hpp
#define qr_data_coder_hpp

#include <stdio.h>
#include <vector>
#include "alphabet.hpp"
#include <opencv2/core.hpp>

typedef std::vector<bool> RowData;

typedef struct Group {
    RowData r1;
    RowData r2;
    RowData r3;
    RowData& row(int n);
    RowData row(int n) const;
} Group;

typedef struct QRData {
    Group g1;
    Group g2;
    Group g3;
    Group g4;
    Group& group(int n);
    Group group(int n) const;
} QRData;


class QRDataCoder {
    Alphabet alphabet;
    cv::Mat grayImg;
public:
    QRDataCoder(int messageSize = 30,
                double correctPercentSize = 0.4);
    std::string decode(QRData &data);
    QRData encode(std::string message);
    
    int getRowSize() const;
    std::string processQRCode(const cv::Mat &colorImg, cv::Mat *warpedQRImage = nullptr);
};


#endif /* qr_data_coder_hpp */
