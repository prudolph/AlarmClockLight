//
//  ViewController.h
//  MyBluefruitTest
//
//  Created by Administrator on 10/8/14.
//  Copyright (c) 2014 BLUECADET. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <CoreBluetooth/CoreBluetooth.h>
#import "UARTPeripheral.h"
@interface ViewController : UIViewController<CBCentralManagerDelegate,UARTPeripheralDelegate>

@property (weak, nonatomic) IBOutlet UIButton *sendDateButton;




typedef enum {
    ConnectionStatusDisconnected = 0,
    ConnectionStatusScanning,
    ConnectionStatusConnected,
} ConnectionStatus;


@property (nonatomic, assign) ConnectionStatus                  connectionStatus;




@end

