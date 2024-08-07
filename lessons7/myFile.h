#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define Size 1024
#define Sign_NOW 1
#define Sign_LINE 2
#define Sign_FULL 3
typedef struct _File{
    int _fd;
    int _flag;
    //刷新策略
    int type;
    //缓冲区
    char buff[Size];
    int cap;
    int size;
}_File;
_File* _fopen(const char*pathname,const char*mode)
{
    int flag=0;
    int defalutMode=0666;
    if(strcmp(mode,"r")==0)
    {
        flag=O_RDONLY;
    }
    else if(strcmp(mode,"w")==0)
    {
        flag=O_WRONLY | O_CREAT |O_TRUNC;
    }
    else if(strcmp(mode,"a")==0)
    {
        flag=O_WRONLY | O_CREAT |O_APPEND;
    }
    else{}
    int fd=0;
    if(flag & O_RDONLY)
    {
        fd = open(pathname,flag);
    }
    else
        fd=open(pathname,flag,defalutMode);
    if(fd<0)
    {
        perror("open");
        exit(2);
    }
    _File* _fp =(_File*) malloc(sizeof(_File));
    _fp->_fd=fd;
    _fp->size=0;
    _fp->cap=Size;
    _fp->type=Sign_LINE;
    memset(_fp->buff,0,Size);
    return _fp;
}
void _fwrite(_File* _fp,void* ptr, int num)
{
    memcpy(_fp->buff+_fp->size,ptr,num);
    _fp->size+=num;
    if(_fp->type==Sign_NOW)
    {
        write(_fp->_fd,_fp->buff,_fp->size);
        _fp->size=0;
    }
    else if(_fp->type & Sign_FULL)
    {
        if(_fp->size==_fp->cap)
        {
            write(_fp->_fd,_fp->buff,_fp->size);
            _fp->size=0;
        }
    }
    else if(_fp->type & Sign_LINE)
    {
        if(_fp->buff[_fp->size-1]=='\0')
        {
            write(_fp->_fd,_fp->buff,_fp->size);
            _fp->size=0;
        }
    }
}
void _fread(_File*_fp,void* ptr,int num)
{
    if(num>_fp->size)
        num = _fp->size;
    memcpy(ptr,_fp->buff,num);
}
void _fflush(_File*_fp)
{
   if(_fp->size)
   {
        write(_fp->_fd,_fp->buff,_fp->size);
   }
}
void _fclose(_File*_fp)
{
    _fflush(_fp);
    close(_fp->_fd);
}
