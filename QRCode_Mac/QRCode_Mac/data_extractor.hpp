//
//  data_extractor.hpp
//  QRCode_Mac
//
//  Created by Alexander Graschenkov on 22/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#ifndef data_extractor_hpp
#define data_extractor_hpp

#include <stdio.h>
#include <opencv2/core.hpp>


typedef std::vector<bool> RowData;

typedef struct Group {
    RowData r1;
    RowData r2;
    RowData r3;
} Group;

typedef struct QRData {
    Group g1;
    Group g2;
    Group g3;
    Group g4;
} QRData;

QRData getQRData(const cv::Mat &grayImg, int rowSize);

#endif /* data_extractor_hpp */
