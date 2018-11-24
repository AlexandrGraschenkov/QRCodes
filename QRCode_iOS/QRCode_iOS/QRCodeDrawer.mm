//
//  QRCodeDrawer.m
//  QRCode_iOS
//
//  Created by Alexander Graschenkov on 24/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#import "QRCodeDrawer.h"
#include "qr_data_coder.hpp"

@implementation QRCodeDrawer
//
//+ (UIBezierPath)bezierRect:(CGRect)rect corners:(CGFloat)corner {
//
//}

+ (void)drawDot:(CGPoint)center context:(CGContextRef)context {
    CGRect rect = CGRectMake(center.x - 30, center.y - 30, 60, 60);
    rect = CGRectInset(rect, 4.5, 4.5);
    UIBezierPath *path = [UIBezierPath bezierPathWithRoundedRect:rect byRoundingCorners:UIRectCornerAllCorners cornerRadii:CGSizeMake(16,16)];
    path.lineWidth = 9;
    
    [path stroke];
    
    UIBezierPath *innerRect = [UIBezierPath bezierPathWithRoundedRect:CGRectMake(center.x - 11, center.y - 11, 22, 22) cornerRadius:5];
    [innerRect fill];
}

+ (void)drawGroup:(Group)group context:(CGContextRef)context {
    CGFloat rowX = 72;
    CGFloat rowWidth = 352 - rowX - rowX;
    UIBezierPath *path = [UIBezierPath new];
    for (int level = 0; level < 3; level++) {
        auto &row = group.row(level);
        int drawFromIdx = -1;
        int levelOffset = 4 + (2-level) * 18;
        for (int i = 0; i < row.size(); i++) {
            if (drawFromIdx < 0 && row[i]) {
                drawFromIdx = i;
            }
            if (i == row.size()-1) {
                int temp = 10;
                temp++;
            }
            if ((!row[i] || i == row.size()-1) && drawFromIdx >= 0) {
                CGFloat x = drawFromIdx * rowWidth / (float)row.size() + rowX;
                int toIdx = row[i] ? i+1 : i;
                CGFloat width = (toIdx - drawFromIdx) * rowWidth / (float)row.size();
                CGRect r = CGRectMake(x, levelOffset, width, 9);
                [path appendPath:[UIBezierPath bezierPathWithRoundedRect:r cornerRadius:5]];
                drawFromIdx = -1;
            }
        }
    }
    [path fill];
}

+ (UIImage *)drawQRMessage:(NSString *)message size:(CGSize)size {
    QRDataCoder coder;
    std::string str(message.UTF8String);
    QRData data = coder.encode(str);
    if (data.g1.r1.size() == 0) {
        return nil;
    }
    
    int side = MIN(size.width, size.height);
    UIGraphicsBeginImageContextWithOptions(size, false, 0);
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    CGSize drawSize = CGSizeMake(352, 352);
//    size = drawSize;
    CGAffineTransform t = CGAffineTransformIdentity;
    t = CGAffineTransformTranslate(t, size.width / 2, size.height / 2 - 22); // strange magic at height???
    t = CGAffineTransformScale(t,   side / sqrt(2) / drawSize.width,
                                    side / sqrt(2) / drawSize.height);
    t = CGAffineTransformRotate(t, M_PI_4);
    t = CGAffineTransformTranslate(t, -size.width / 2, -size.height / 2);
    CGContextConcatCTM(context, t);
    
    [[UIColor blackColor] set];
    
    for (int i = 0; i < 4; i++) {
        CGPoint p = CGPointMake(30, 30);
        if (i == 1 || i == 2) {
            p.x = drawSize.width - p.x;
        }
        if (i == 2 || i == 3) {
            p.y = drawSize.height - p.y;
        }
        [self drawDot:p context:context];
    }
    
    for (int i = 0; i < 4; i++) {
        CGContextSaveGState(context);
        if (i > 0) {
            CGAffineTransform t = CGAffineTransformIdentity;
            t = CGAffineTransformTranslate(t, drawSize.width / 2, drawSize.height / 2);
            t = CGAffineTransformRotate(t, i * M_PI_2);
            t = CGAffineTransformTranslate(t, -drawSize.width / 2, -drawSize.height / 2);
            CGContextConcatCTM(context, t);
        }
        [self drawGroup:data.group(i) context:context];
        CGContextRestoreGState(context);
    }
//    CGContextSetLineWidth(context, 5);
//    CGContextStrokeRect(context, CGRectMake(0, 0, drawSize.width, drawSize.height));
    
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return image;
}

@end
