//
//  ViewController.m
//  QRCode_iOS
//
//  Created by Alexander Graschenkov on 13/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#import "ViewController.h"
#import "Camera.h"
#import "UIView+Mat.h"
#import <opencv2/imgproc.hpp>
#include "fps.hpp"
#include "qr_data_coder.hpp"

@interface ViewController () <CameraDelegate> {
    Camera *cam;
    FPS *fps;
    FPSIteration *fpsIter;
    QRDataCoder coder;
}
@property (atomic, assign) BOOL captureNextImage;
@property (nonatomic, weak) IBOutlet UIView *previewView;
@property (nonatomic, weak) IBOutlet UILabel *recogLabel;
@property (nonatomic, weak) IBOutlet UILabel *fpsLabel;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
#if TARGET_OS_IPHONE
    cam = [[Camera alloc] init];
    cam.delegate = self;
    [self.view.layer insertSublayer:cam.previewLayer atIndex:0];
    cam.previewLayer.frame = self.view.bounds;
#endif
    
    fps = new FPS();
    fpsIter = new FPSIteration();
}

- (void)viewDidLayoutSubviews {
    [super viewDidLayoutSubviews];
    
    cam.previewLayer.frame = self.view.bounds;
}

- (IBAction)dismissPressed:(id)sender {
    [self dismissViewControllerAnimated:true completion:nil];
}

- (void)cameraImageRecordered:(cv::Mat&)img {
    fpsIter->start();
    cv::Mat outMat;
    std::string str = coder.processQRCode(img, &outMat);
    fpsIter->end();
    fps->tick();
    
    if (outMat.cols) {
        [self.previewView displayContentMat:outMat];
    }
    NSString *dispStr = [NSString stringWithUTF8String:str.c_str()];
    
    double fpsVal = fps->getFPS();
    double fpsProcess = fpsIter->getFPS();
    dispatch_async(dispatch_get_main_queue(), ^{
        if (dispStr.length) {
            self.recogLabel.text = dispStr;
        } else {
            self.recogLabel.text = @"-";
        }
        
        self.fpsLabel.text = [NSString stringWithFormat:@"Fps: %.2lf; Process: %.2lf", fpsVal, fpsProcess];
    });
}

@end
