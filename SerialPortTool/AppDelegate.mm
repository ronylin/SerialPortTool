//
//  AppDelegate.m
//  SerialPortTool
//
//  Created by Rony on 17/3/31.
//  Copyright © 2017年 Rony. All rights reserved.
//

#import "AppDelegate.h"
#import <sys/types.h>
#import <dirent.h>
#import "sys/ioctl.h"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    [txtbdrate setStringValue:@"9600"];
    [txtparity setStringValue:@"N"];
    [txtdatabits setStringValue:@"8"];
    [txtstopbits setStringValue:@"1"];
    [self btScan:nil];
    [combdevice selectItemAtIndex:1];
    [btclose setEnabled:NO];
//    NSLog(@"enterkey:%s",[btsend keyEquivalent]);
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

-(id)init
{
    if(self=[super init])
    {
        exitthread=true;
    }
    return self;
}


-(void)dealloc
{
    if(csp) delete csp;
    if(m_readthread) [m_readthread release];
    [super dealloc];
}

-(void)btScan:(id)sender
{
    const char dir[]="/dev/";
    DIR* dr=opendir(dir);
    struct dirent* fileinfor;
    [combdevice removeAllItems];
    
    while((fileinfor=readdir(dr)))
          {
              NSString *filename=[NSString stringWithUTF8String:fileinfor->d_name];
              if([filename rangeOfString:@"tty."].location!=NSNotFound)
                  [combdevice addItemWithObjectValue:[NSString stringWithFormat:@"/dev/%@",filename]];
          }
 
}


-(void)readDataInBackground
{
    int bytes,fds=csp->fd;
    
    int err;
    while (!exitthread)
    {
        err=ioctl(fds, FIONREAD, &bytes);

        if(err<0)
        {
            NSDateFormatter *fmt=[[NSDateFormatter alloc]init];
            [fmt setDateFormat:@"MM-dd hh:mm:ss"];
            NSString* timestr=[fmt stringFromDate:[NSDate date]];
            [fmt release];
            NSString *str=[NSString stringWithFormat:@"%@ : %@\n",timestr,@"error encountered when reading data!"];
            [[[txthistory textStorage] mutableString]appendString:str];
            
        }
        else if(bytes>0)
        {
            NSDateFormatter *fmt=[[NSDateFormatter alloc]init];
            [fmt setDateFormat:@"MM-dd hh:mm:ss"];
            NSString* timestr=[fmt stringFromDate:[NSDate date]];
            [fmt release];
            size_t length=csp->readString(buffer, 1024*1024);
            NSLog(@"received data of length:%zu",length);
            NSString *str=[NSString stringWithFormat:@"%@ received: %@\n",timestr,[NSString stringWithUTF8String:buffer]];
            [[[txthistory textStorage]mutableString]appendString:str];
            [self performSelectorOnMainThread:@selector(display) withObject:nil waitUntilDone:YES];

        }
        [NSThread sleepForTimeInterval:0.2];

    }
}

-(void)display
{
    [txthistory setNeedsDisplay:YES];
    [txthistory scrollToEndOfDocument:nil];
}

-(IBAction)btOpen:(id)sender
{
    if(csp)
    {
        delete csp;
        csp=nil;
    }
    
    if(m_readthread)
    {
        [m_readthread cancel];
        [m_readthread release];
    }
    [btopen setEnabled:NO];
    [btclose setEnabled:YES];
    csp=new CSerialPort();
    bool opensuccess=csp->open([[combdevice objectValueOfSelectedItem]UTF8String]);
    
    NSDateFormatter* fmt=[[NSDateFormatter alloc]init];
    [fmt setDateFormat:@"MM-dd hh:mm:ss"];
    NSString *timestr=[fmt stringFromDate:[NSDate date]];
    [fmt release];
    NSString *str=[NSString stringWithFormat:@"%@ : %@\n",timestr,@"Serial Port Opened!"];
    if(opensuccess) [[[txthistory textStorage]mutableString]appendString:str];
    
    int bdr=[[txtbdrate stringValue]intValue];
    int databits=[[txtdatabits stringValue]intValue];
    int stopbits=[[txtstopbits stringValue]intValue];
    char parity=*[[txtstopbits stringValue] UTF8String];
    csp->setPort(bdr, databits, stopbits, parity);
    
    m_readthread=[[NSThread alloc]initWithTarget:self selector:@selector(readDataInBackground) object:nil];
    exitthread=false;
    [m_readthread start];
    [self display];
}

-(IBAction)btClose:(id)sender
{
    exitthread=true;
    [m_readthread cancel];
    [m_readthread release];
    m_readthread=nil;
    if(csp)
    {
        csp->close();
        delete csp;
        csp=nil;
    }
    
    NSDateFormatter *fmt=[[NSDateFormatter alloc]init];
    [fmt setDateFormat:@"MM-dd hh:mm:ss"];
    NSString *timestr=[fmt stringFromDate:[NSDate date]];
    NSString *str=[NSString stringWithFormat:@"%@ : %@\n",timestr,@"Serial Port Closed!"];
    [fmt release];
    [[[txthistory textStorage]mutableString]appendString:str];
    [btclose setEnabled:NO];
    [btopen setEnabled:YES];
    [self display];
}

-(void)writeString:(NSString*)str
{
    csp->writeString([str UTF8String]);
}

-(IBAction)btSend:(id)sender
{
    NSDateFormatter *fmt=[[NSDateFormatter alloc]init];
    [fmt setDateFormat:@"MM-dd hh:mm:ss"];
    NSString *timestr=[fmt stringFromDate:[NSDate date]];
    [fmt release];

    if(csp)
    {
        NSString *str=[NSString stringWithFormat:@"%@ send: %@\n",timestr,[txtSend stringValue]];
        [[[txthistory textStorage]mutableString]appendString:str];
        
        [self performSelectorInBackground:@selector(writeString:) withObject:str];
        
        
    }else{
        NSString* str=[NSString stringWithFormat:@"%@ : %@\n",timestr,@"No Serial Port opened!"];
        [[[txthistory textStorage]mutableString]appendString:str];
    }
    
    [txtSend setStringValue:@""];
    [self display];
}



@end
