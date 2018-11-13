//
//  UIView+Mat.m
//  QRCode_iOS
//
//  Created by Alexander Graschenkov on 13/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#import "UIView+Mat.h"

@implementation UIView (Mat)

- (void)displayContentMat:(cv::Mat)image {
    CGImage* dstImage;
    
    CGColorSpaceRef colorSpace;
    CGContextRef context;
    
    // check if matrix data pointer or dimensions were changed by the delegate
    bool iOSimage = true;
    
    // (create color space, create graphics context, render buffer)
    CGBitmapInfo bitmapInfo;
    
    // basically we decide if it's a grayscale, rgb or rgba image
    if (image.channels() == 1) {
        colorSpace = CGColorSpaceCreateDeviceGray();
        bitmapInfo = kCGImageAlphaNone;
    } else if (image.channels() == 3) {
        colorSpace = CGColorSpaceCreateDeviceRGB();
        bitmapInfo = kCGImageAlphaNone;
        if (iOSimage) {
            bitmapInfo |= kCGBitmapByteOrder32Little;
        } else {
            bitmapInfo |= kCGBitmapByteOrder32Big;
        }
    } else {
        colorSpace = CGColorSpaceCreateDeviceRGB();
        bitmapInfo = kCGImageAlphaPremultipliedFirst;
        if (iOSimage) {
            bitmapInfo |= kCGBitmapByteOrder32Little;
        } else {
            bitmapInfo |= kCGBitmapByteOrder32Big;
        }
    }
    
    context = CGBitmapContextCreate(image.data, image.cols, image.rows, 8, image.step1(), colorSpace, bitmapInfo);
    dstImage = CGBitmapContextCreateImage(context);
    CGContextRelease(context);
    
    // render buffer
    if ([NSThread isMainThread]) {
        self.layer.contents = (__bridge id)dstImage;
    } else {
        dispatch_sync(dispatch_get_main_queue(), ^{
            self.layer.contents = (__bridge id)dstImage;
        });
    }
    
    // cleanup
    CGImageRelease(dstImage);
    
    CGColorSpaceRelease(colorSpace);
}

@end
