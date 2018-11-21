//
//  dot_detector.hpp
//  QRCode_Mac
//
//  Created by Alexander Graschenkov on 21/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#ifndef dot_detector_hpp
#define dot_detector_hpp

#include <stdio.h>
#include <opencv2/core.hpp>


typedef enum OneRowType {
    OneRowTypeVertical,
    OneRowTypeHorisontal,
    OneRowTypeDiagonal1,
    OneRowTypeDiagonal2
} OneRowType;

/// Переводим нашу матрицу в однострочный формат
/// rowIdxOut: какой индекс будет у переданной позиции пикселя
cv::Mat const getOneColumnMat(cv::Mat const &mat,
                              int row,
                              int column,
                              OneRowType type,
                              int &rowIdxOut);

typedef struct Dot {
    cv::Point2f pos;
    cv::Rect area;
    
    Dot(cv::Point2f pos, cv::Rect area):
        pos(pos), area(area) {}
} Dot;

std::vector<Dot> findDots(const cv::Mat &grayImg);

#endif /* dot_detector_hpp */
