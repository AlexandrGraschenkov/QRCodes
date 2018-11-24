//
//  QRCodeDrawer.h
//  QRCode_iOS
//
//  Created by Alexander Graschenkov on 24/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface QRCodeDrawer : NSObject
+ (UIImage *)drawQRMessage:(NSString *)message size:(CGSize)size;
@end

NS_ASSUME_NONNULL_END
