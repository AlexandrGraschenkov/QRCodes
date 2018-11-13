//
//  UIView+Mat.h
//  QRCode_iOS
//
//  Created by Alexander Graschenkov on 13/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <opencv2/core.hpp>

NS_ASSUME_NONNULL_BEGIN

@interface UIView (Mat)

- (void)displayContentMat:(cv::Mat)mat;

@end

NS_ASSUME_NONNULL_END
