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
    CGColorSpaceRef colorSpace;
    
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
        bitmapInfo = kCGBitmapByteOrder32Little | kCGImageAlphaNone;
    } else {
        colorSpace = CGColorSpaceCreateDeviceRGB();
        bitmapInfo = kCGImageAlphaPremultipliedFirst;
        if (iOSimage) {
            bitmapInfo |= kCGBitmapByteOrder32Little;
        } else {
            bitmapInfo |= kCGBitmapByteOrder32Big;
        }
    }
    
    NSData *data = [NSData dataWithBytes:image.data length:image.step.p[0]*image.rows];
    CGDataProviderRef provider = CGDataProviderCreateWithCFData((__bridge CFDataRef)data);
    CGImageRef imageRef = CGImageCreate(
                                        image.cols,                 //width
                                        image.rows,                 //height
                                        8,                          //bits per component
                                        8 * image.elemSize(),       //bits per pixel
                                        image.step[0],              //bytesPerRow
                                        colorSpace,                 //colorspace
                                        bitmapInfo,                 // bitmap info
                                        provider,                   //CGDataProviderRef
                                        NULL,                       //decode
                                        false,                      //should interpolate
                                        kCGRenderingIntentDefault   //intent
                                        );
    
    // render buffer
    if ([NSThread isMainThread]) {
        self.layer.contents = (__bridge id)imageRef;
    } else {
        dispatch_sync(dispatch_get_main_queue(), ^{
            self.layer.contents = (__bridge id)imageRef;
        });
    }
    
    // cleanup
    CGImageRelease(imageRef);
    
    CGColorSpaceRelease(colorSpace);
}

@end
