//
//  CSerialPort.hpp
//  SerialPort
//
//  Created by Apple on 17/3/30.
//  Copyright © 2017年 Apple. All rights reserved.
//

#ifndef CSerialPort_h
#define CSerialPort_h

#import <stdio.h>
#import <pthread.h>

typedef void (*callback)(void *data,size_t datalength);

class CSerialPort
{
public:
    CSerialPort();
    ~CSerialPort();
public:
    int fd;
    callback m_callbackfunc;
    char buffer[1024*1024];

private:
    bool exitthread;
    pthread_t thd;

public:
    bool open(const char *dev);
    int close();
    void onReceivedData(void* data,size_t length);
    void setcallbackfunc(callback cbfunc);
    size_t readString(char *bufter,size_t maxsize);
    size_t writeString(const char* str);
    bool setPort(int baudrate,int databits,int stopbit,char parity);
    int openReadThread();
    static void* readDataInBackground(void* arg);

};


#endif /* CSerialPort_h */
