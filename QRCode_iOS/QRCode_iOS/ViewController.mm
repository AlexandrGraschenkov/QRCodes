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

@interface ViewController () <CameraDelegate> {
    Camera *cam;
}
@property (atomic, assign) BOOL captureNextImage;
@property (nonatomic, weak) IBOutlet UIView *previewView;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    cam = [[Camera alloc] init];
    cam.delegate = self;
    [self.view.layer insertSublayer:cam.previewLayer atIndex:0];
    cam.previewLayer.frame = self.view.bounds;
}

- (void)viewDidLayoutSubviews {
    [super viewDidLayoutSubviews];
    
    cam.previewLayer.frame = self.view.bounds;
}

- (IBAction)processButtonPressed:(id)sender {
    self.captureNextImage = true;
}

- (void)cameraImageRecordered:(cv::Mat&)img {
    if (!self.captureNextImage) return;
    
    cv::Mat outMat;
    cv::Canny(img, outMat, 100, 160);
    self.captureNextImage = false;
    [self.previewView displayContentMat:outMat];
}

@end
