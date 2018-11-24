//
//  QRDisplayController.m
//  QRCode_iOS
//
//  Created by Alexander Graschenkov on 24/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#import "QRDisplayController.h"
#include "QRCodeDrawer.h"

@interface QRDisplayController ()
{}
@property (nonatomic, weak) IBOutlet UIImageView *imgView;
@property (nonatomic, weak) IBOutlet UITextField *textField;
@end

@implementation QRDisplayController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (IBAction)generatePressed:(id)sender {
    self.imgView.image = [QRCodeDrawer drawQRMessage:self.textField.text size:CGSizeMake(300, 300)];
}

@end
