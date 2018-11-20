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


std::vector<cv::Point2f> findDots(const cv::Mat &grayImg);

#endif /* dot_detector_hpp */
