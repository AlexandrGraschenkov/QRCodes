//
//  process_qr.hpp
//  QRCode_iOS
//
//  Created by Alexander Graschenkov on 24/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#ifndef process_qr_hpp
#define process_qr_hpp

#include <stdio.h>
#include "qr_data_coder.hpp"
#include <opencv2/core.hpp>

std::string processQRCode(const cv::Mat &colorImg, cv::Mat *warpedQRImage = nullptr);

#endif /* process_qr_hpp */
