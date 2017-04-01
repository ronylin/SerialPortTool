//
//  CSerialPort.cpp
//  SerialPort
//
//  Created by Rony on 17/3/30.
//  Copyright © 2017年 Apple. All rights reserved.
//

#import "CSerialPort.h"
#import <termios.h>
#import <fcntl.h>
#import <unistd.h>
#import <iostream>
#import <sys/ioctl.h>
using namespace std;

CSerialPort::CSerialPort()
{
    fd=-1;
    exitthread=true;
}

CSerialPort::~CSerialPort()
{
    fd=-1;
    exitthread=true;
}

bool CSerialPort::open(const char * dev)
{
    fd=::open(dev,O_RDWR | O_NOCTTY | O_NONBLOCK);
    
    if(fd==-1)
    {
        cout<<"failed openning serial port:"<<dev<<endl;
        return false;
    
    }else{
        cout<<"opened serial port:"<<dev<<" successfully!"<<endl;
        
        if(fcntl(fd, F_SETFL,0)<0)
            cout<<"failed set flag"<<endl;
        else
            cout<<"set flag successfully"<<endl;

        if(isatty(STDIN_FILENO)==0)
            printf("standard input is not a terminal device\n");
        else
            printf("isatty success!\n");
        
        exitthread=false;
        return true;

    }
}

int CSerialPort::close()
{
        exitthread=true;
        return ::close(fd);
}

void * CSerialPort::readDataInBackground(void* arg)
{
    CSerialPort *cport=(CSerialPort*)arg;
    int bytes,fds=cport->fd;
    int err;
    while (!cport->exitthread)
    {
        err=ioctl(fds, FIONREAD, &bytes);
        if(err<0)
            cout<<"error encountered when reading data!"<<endl;
        else if(bytes>0)
        {
            size_t length=cport->readString(cport->buffer, 1024*1024);
            cport->onReceivedData(cport->buffer,length);
            cout<<"read"<<length<<" data in background:\n"<<cport->buffer<<endl;
        }
        usleep(100);
    }
    return 0;
    
}


int CSerialPort::openReadThread()
{
    int ret=pthread_create(&thd, NULL, readDataInBackground, this);
    if (ret==0)
        cout<<"open read thread successfully!"<<endl;
    else
        cout<<"open read thread failed!"<<endl;

    return ret;
}


void CSerialPort::setcallbackfunc(callback cbfunc)
{
    m_callbackfunc=cbfunc;
}

void CSerialPort::onReceivedData(void* data,size_t length)
{
    if(m_callbackfunc) m_callbackfunc(data,length);
}

size_t CSerialPort::readString(char *bufter,size_t maxsize)
{
    size_t ret=1,remain=maxsize-ret;
    memset(bufter, 0, maxsize);
    char *p=bufter;
    while(ret!=0)
    {
        ret=read(fd, p, remain);
        p=p+ret;
        remain-=ret;
    }
    *p='\0';
    return maxsize-remain;
}

size_t CSerialPort::writeString(const char* str)
{
    size_t bytes=strlen(str);
    ssize_t ret=write(fd, str, bytes);
   if(ret<0)
       cout<<"write failed with error:"<<errno<<endl;
    else
        cout<<"write successfully of bytes:"<<ret<<endl;
    return ret;
}

bool CSerialPort::setPort(int baudrate,int databits,int stopbit,char parity)
{
    struct termios newtio,oldtio;
    if(tcgetattr(fd,&oldtio)!=0)
    {
        cout<<"fail reading attribute of fd "<<endl;
    }
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag |=CLOCAL|CREAD;
    newtio.c_cflag &=~CSIZE;
    switch (databits) {
        case 8:
            newtio.c_cflag |= CS8;
            break;
            
        default:
            break;
    }
    switch (parity) {
        case 'N':
            newtio.c_cflag &= ~PARENB;
            break;
            
        default:
            break;
    }
    switch (baudrate) {
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
            break;
        default:
            break;
    }
    switch (stopbit) {
        case 1:
            newtio.c_cflag &= ~CSTOPB;
            break;
        case 2:
            newtio.c_cflag |= CSTOPB;
            break;
    
        default:
            break;
    }
    newtio.c_cc[VTIME]=0;
    newtio.c_cc[VMIN]=0;
    fcntl(fd, F_SETFL, 0);//block
    tcflush(fd, TCIFLUSH);
    if(tcsetattr(fd, TCSANOW, &newtio)!=0)
        cout<<"set port failed"<<endl;
    else
        cout<<"set port successfully!"<<endl;
    return true;
}

