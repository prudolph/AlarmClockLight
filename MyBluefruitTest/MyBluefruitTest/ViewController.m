//
//  ViewController.m
//  MyBluefruitTest
//
//  Created by Administrator on 10/8/14.
//  Copyright (c) 2014 BLUECADET. All rights reserved.
//

#import "ViewController.h"
#import "NSString+hex.h"
#import "NSData+hex.h"
@interface ViewController (){
    CBCentralManager    *cm;

    UARTPeripheral      *currentPeripheral;
}
@end

@implementation ViewController
            
- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    
      cm = [[CBCentralManager alloc] initWithDelegate:self queue:nil];
    
  
   
    _connectionStatus = ConnectionStatusDisconnected;
  //  [self scanForPeripherals];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


- (void)scanForPeripherals{
    
    NSLog(@"Scanning for Peripherals");
    //Look for available Bluetooth LE devices
    
    //skip scanning if UART is already connected
    NSArray *connectedPeripherals = [cm retrieveConnectedPeripheralsWithServices:@[UARTPeripheral.uartServiceUUID]];
    if ([connectedPeripherals count] > 0) {
        //connect to first peripheral in array
        [self connectPeripheral:[connectedPeripherals objectAtIndex:0]];
    }
    
    else{
        
        [cm scanForPeripheralsWithServices:@[UARTPeripheral.uartServiceUUID]
                                   options:@{CBCentralManagerScanOptionAllowDuplicatesKey: [NSNumber numberWithBool:NO]}];
    }
}


- (void)connectPeripheral:(CBPeripheral*)peripheral{
    
    NSLog(@"Connect Peripheral");
    //Connect Bluetooth LE device
    
    //Clear off any pending connections
    [cm cancelPeripheralConnection:peripheral];
    
    //Connect
    currentPeripheral = [[UARTPeripheral alloc] initWithPeripheral:peripheral delegate:self];
    [cm connectPeripheral:peripheral options:@{CBConnectPeripheralOptionNotifyOnDisconnectionKey: [NSNumber numberWithBool:YES]}];
    
}


- (void)disconnect{
    
    //Disconnect Bluetooth LE device
    
    _connectionStatus = ConnectionStatusDisconnected;
    
    [cm cancelPeripheralConnection:currentPeripheral.peripheral];
    
}


#pragma mark CBCentralManagerDelegate


- (void) centralManagerDidUpdateState:(CBCentralManager*)central{
    NSLog(@"centralManagerDidUpdateState");
    if (central.state == CBCentralManagerStatePoweredOn){
        NSLog(@"State is ON");
        [self scanForPeripherals];
    }
    
    else if (central.state == CBCentralManagerStatePoweredOff){
        NSLog(@"State is OFF");
        //respond to powered off
    }
    
}


- (void) centralManager:(CBCentralManager*)central didDiscoverPeripheral:(CBPeripheral*)peripheral advertisementData:(NSDictionary*)advertisementData RSSI:(NSNumber*)RSSI{
    
    NSLog(@"Did discover peripheral %@", peripheral.name);
    
    [cm stopScan];
    
    [self connectPeripheral:peripheral];
}


- (void) centralManager:(CBCentralManager*)central didConnectPeripheral:(CBPeripheral*)peripheral{
        NSLog(@"didConnectPeripheral");
    if ([currentPeripheral.peripheral isEqual:peripheral]){
        
        if(peripheral.services){
            NSLog(@"Did connect to existing peripheral %@", peripheral.name);
            [currentPeripheral peripheral:peripheral didDiscoverServices:nil]; //already discovered services, DO NOT re-discover. Just pass along the peripheral.
        }
        
        else{
            NSLog(@"Did connect peripheral %@", peripheral.name);
            [currentPeripheral didConnect];
        }
    }
}


- (void) centralManager:(CBCentralManager*)central didDisconnectPeripheral:(CBPeripheral*)peripheral error:(NSError*)error{
    
    NSLog(@"Did disconnect peripheral %@", peripheral.name);
    
    //respond to disconnected
    [self peripheralDidDisconnect];
    
    if ([currentPeripheral.peripheral isEqual:peripheral])
    {
        [currentPeripheral didDisconnect];
    }
}


#pragma mark UARTPeripheralDelegate


- (void)didReadHardwareRevisionString:(NSString*)string{
    
    //Once hardware revision string is read, connection to Bluefruit is complete
    
    NSLog(@"DidReadHardwareRevisionString:: HW Revision: %@", string);
    

    _connectionStatus = ConnectionStatusConnected;
    
    
    //Send Over the date as a string
    

    
    

}


- (void)uartDidEncounterError:(NSString*)error{
    
    NSLog(@"Uart encounterd an error");
    
}


- (void)didReceiveData:(NSData*)newData{
    
    //Data incoming from UART peripheral, forward to current view controller
    
    if (_connectionStatus == ConnectionStatusConnected || _connectionStatus == ConnectionStatusScanning) {
        //UART
        
        //Debug
            NSString *hexString = [newData hexRepresentationWithSpaces:YES];
            NSLog(@"Received: %@", newData);
    
        
        
    }
}


- (void)peripheralDidDisconnect{
    
    //respond to device disconnecting
    
    //if we were in the process of scanning/connecting, dismiss alert
   
        [self uartDidEncounterError:@"Peripheral disconnected"];
   
    
    //if status was connected, then disconnect was unexpected by the user, show alert
   
    
    
    _connectionStatus = ConnectionStatusDisconnected;

    NSLog(@"Periphial did disconnect");
    }


- (void)alertBluetoothPowerOff{
    
    //Respond to system's bluetooth disabled
    
    NSString *title     = @"Bluetooth Power";
    NSString *message   = @"You must turn on Bluetooth in Settings in order to connect to a device";
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:title message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alertView show];
}


- (void)alertFailedConnection{
    
    //Respond to unsuccessful connection
    
    NSString *title     = @"Unable to connect";
    NSString *message   = @"Please check power & wiring,\nthen reset your Arduino";
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:title message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alertView show];
    
}


#pragma mark UartViewControllerDelegate / PinIOViewControllerDelegate


- (void)sendData:(NSData*)newData{
    
    //Output data to UART peripheral
    
    NSString *hexString = [newData hexRepresentationWithSpaces:YES];
    NSLog(@"Sending: %@", hexString);
    
    [currentPeripheral writeRawData:newData];
    
}



#pragma mark - UICOntrolls


-(IBAction)sendDate:(id)sender{

  
    

    
    NSDateFormatter *formatter = [[NSDateFormatter alloc]init];
    [formatter setDateFormat:@"MM/dd/yy hh:mm:ss a"];
    NSTimeZone *timeZone = [NSTimeZone timeZoneWithAbbreviation:@"EST"];
    [formatter setTimeZone:timeZone];
   NSString *dateAsString =  [formatter stringFromDate:[NSDate date]];

      NSLog(@"Sending the current Date %@ to arduino", dateAsString);
    
    //Send inputField's string via UART
    NSData *data = [NSData dataWithBytes:dateAsString.UTF8String length:dateAsString.length];
    [self sendData:data];
   
    
    
    
    
}

@end
