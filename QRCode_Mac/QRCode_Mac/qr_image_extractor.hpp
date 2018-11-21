//
//  qr_image_extractor.hpp
//  QRCode_Mac
//
//  Created by Alexander Graschenkov on 21/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#ifndef qr_image_extractor_hpp
#define qr_image_extractor_hpp

#include <stdio.h>
#include <opencv2/core.hpp>

#include "dot_detector.hpp"

cv::Mat extractQR(const cv::Mat &grayImg, std::vector<Dot> dots);

#endif /* qr_image_extractor_hpp */
