//
//  AppDelegate.h
//  SerialPortTool
//
//  Created by Apple on 17/3/31.
//  Copyright © 2017年 Rony. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CSerialPort.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>
{
    IBOutlet NSComboBox* combdevice;
    IBOutlet NSTextField* txtSend;
    IBOutlet NSTextField* txtbdrate;
    IBOutlet NSTextField* txtdatabits;
    IBOutlet NSTextField* txtparity;
    IBOutlet NSTextField* txtstopbits;
    IBOutlet NSTextView* txthistory;
    IBOutlet NSButton* btopen;
    IBOutlet NSButton* btclose;
    IBOutlet NSButton* btsend;

    
    CSerialPort* csp;
    NSThread* m_readthread;
    bool exitthread;
    char buffer[1024*1024];
}
-(IBAction)btScan:(id)sender;
-(IBAction)btOpen:(id)sender;
-(IBAction)btClose:(id)sender;
-(IBAction)btSend:(id)sender;

@end

