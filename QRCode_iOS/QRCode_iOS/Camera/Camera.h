//
//  Camera.h
//  Recorder
//
//  Created by Alexander Graschenkov on 26.02.18.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import <opencv2/core.hpp>


@protocol CameraDelegate
- (void)cameraImageRecordered:(cv::Mat&)img;
@end

@interface Camera : NSObject

- (instancetype)init;

@property (nonatomic, weak) id<CameraDelegate> delegate;

@property (nonatomic, readonly) AVCaptureVideoPreviewLayer *previewLayer;

@end
