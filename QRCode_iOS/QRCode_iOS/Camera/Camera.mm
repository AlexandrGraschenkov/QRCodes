//
//  Camera.m
//  QRCode_iOS
//
//  Created by Alexander Graschenkov on 13.11.18.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#import "Camera.h"

#define kOrientation AVCaptureVideoOrientationPortrait

@interface Camera() <AVCaptureVideoDataOutputSampleBufferDelegate, AVCaptureAudioDataOutputSampleBufferDelegate> {
    AVCaptureSession *session;
    
    AVCaptureVideoPreviewLayer *layer;
    dispatch_queue_t bgQueue;
    
    AVCaptureConnection *audioConnection;
    AVCaptureConnection *videoConnection;
}
@end

@implementation Camera

- (instancetype)init {
    self = [super init];
    if (self) {
        bgQueue = dispatch_queue_create("cameraQueue", NULL);
        [self setup];
        
        layer = [AVCaptureVideoPreviewLayer layerWithSession:session];
        layer.videoGravity = AVLayerVideoGravityResizeAspectFill;
        layer.connection.videoOrientation = kOrientation;
        
        [session startRunning];
    }
    return self;
}

- (void)dealloc {
    [layer removeFromSuperlayer];
}

- (AVCaptureVideoPreviewLayer *)previewLayer {
    return layer;
}

#pragma mark - Pirvate

- (void)setup {
    session = [AVCaptureSession new];
    [session beginConfiguration];
    [self setupVideo];
    [self setupAudio];
    [session commitConfiguration];
}

- (AVCaptureDevice *)backCamera {
    static AVCaptureDevice *backCamera = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        for (AVCaptureDevice *device in [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo]) {
            if (device.position == AVCaptureDevicePositionBack) {
                backCamera = device;
                break;
            }
        }
    });
    return backCamera;
}

- (void)setupVideo {
#if TARGET_IPHONE_SIMULATOR
    return;
#endif
    int fps = 60;
    AVCaptureDevice *backCamera = [self backCamera];
    [session addInput:[AVCaptureDeviceInput deviceInputWithDevice:backCamera error:nil]];
    
    
    AVCaptureVideoDataOutput *videoOut = [[AVCaptureVideoDataOutput alloc] init];
    videoOut.alwaysDiscardsLateVideoFrames = YES;
    [videoOut setSampleBufferDelegate:self queue:bgQueue];
    
    // Set the video output to store frame in BGRA (It is supposed to be faster)
//    videoOut.videoSettings = @{(id)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_32BGRA)};
    videoOut.videoSettings = @{(id)kCVPixelBufferPixelFormatTypeKey : @(kCVPixelFormatType_420YpCbCr8BiPlanarFullRange)};

    [session addOutput:videoOut];
    [session setSessionPreset:AVCaptureSessionPreset1280x720];
    videoConnection = [videoOut connectionWithMediaType:AVMediaTypeVideo];
    videoConnection.videoOrientation = kOrientation;
    videoConnection.preferredVideoStabilizationMode = AVCaptureVideoStabilizationModeOff;
    
    // Setup camera capture mode
    for(AVCaptureDeviceFormat *vFormat in [backCamera formats] ) {
        CMFormatDescriptionRef description= vFormat.formatDescription;
        CMVideoDimensions dim = CMVideoFormatDescriptionGetDimensions(description);
        float maxRate = ((AVFrameRateRange*) [vFormat.videoSupportedFrameRateRanges objectAtIndex:0]).maxFrameRate;
        if (maxRate >= fps && dim.width == 1280 && CMFormatDescriptionGetMediaSubType(description)==kCVPixelFormatType_420YpCbCr8BiPlanarFullRange) {
            if ([backCamera lockForConfiguration:nil]) {
                NSLog(@"Set camera capture mode %@", vFormat);
                backCamera.activeFormat = vFormat;
                backCamera.activeVideoMinFrameDuration = CMTimeMake(100,100 * fps);
                backCamera.activeVideoMaxFrameDuration = CMTimeMake(100,100 * fps);
                [backCamera unlockForConfiguration];
                break;
            }
        }
    }
}

- (void)setupAudio {
    AVCaptureDevice *audioDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeAudio];
    AVCaptureDeviceInput *audioIn = [[AVCaptureDeviceInput alloc] initWithDevice:audioDevice error:nil];
    if ( [session canAddInput:audioIn] ) {
        [session addInput:audioIn];
    }
    
    AVCaptureAudioDataOutput *audioOut = [[AVCaptureAudioDataOutput alloc] init];
    [audioOut setSampleBufferDelegate:self queue:bgQueue];
    
    if ( [session canAddOutput:audioOut] ) {
        [session addOutput:audioOut];
    }
    audioConnection = [audioOut connectionWithMediaType:AVMediaTypeAudio];
}

#pragma mark - AVCaptureSession delegate
- (void)captureOutput:(AVCaptureOutput *)captureOutput
didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
       fromConnection:(AVCaptureConnection *)connection {
    
    if (connection == videoConnection) {
        // image data from camera
        CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
        CVPixelBufferLockBaseAddress(imageBuffer, 0);
        
        void* bufferAddress;
        size_t width;
        size_t height;
        size_t bytesPerRow;
        
        int format_opencv;
        
        format_opencv = CV_8UC1;
        
        bufferAddress = CVPixelBufferGetBaseAddressOfPlane(imageBuffer, 0);
        width = CVPixelBufferGetWidthOfPlane(imageBuffer, 0);
        height = CVPixelBufferGetHeightOfPlane(imageBuffer, 0);
        bytesPerRow = CVPixelBufferGetBytesPerRowOfPlane(imageBuffer, 0);
        
        cv::Mat image((int)height, (int)width, format_opencv, bufferAddress, bytesPerRow);
        [self.delegate cameraImageRecordered:image];
        
        CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
    } else if (connection == audioConnection) {
        // audio data
    }
}
@end
