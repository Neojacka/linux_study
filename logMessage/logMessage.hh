#include<iostream>
#include<stdarg.h>
#include<ctime>
#include<cstdio>
#include<cstring>

#include<unistd.h>
#define DEBUG 1
#define NORMAL 2
#define WARNING 3
#define ERROR 4
#define FATAL 5
#define  DEV /dev/null
#define LOG_NORMAL "LOG_NORMAL.txt"
#define LOG_ERR "LOG_ERR.txx"
using namespace std;

const char *to_level(int level)
{
    switch (level)
    {
    case DEBUG:
        return "DEBUG";
    case NORMAL:
        return "NORMAL";
    case WARNING:
        return "WARNING";
    case ERROR:
        return "ERROR";
    case FATAL:
        return "FATAL";
    }
}

void logMessage(int level,const char*format,...)
{
    time_t t=time(nullptr);
    char bufftime[100];
    strftime(bufftime,sizeof(bufftime),"%Y-%m-%d %H:%M:%S",localtime(&t));
#define NUM 1024
    char logpre[NUM];
    snprintf(logpre,sizeof(logpre),"[%s][%s][pid:%d]",to_level(level),bufftime,getpid());
    va_list agc;
    va_start(agc,format);
    char logcontext[NUM];
    vsnprintf(logcontext,sizeof(logcontext),format,agc);
    FILE* log=fopen(LOG_NORMAL,"a");
    FILE* err=fopen(LOG_ERR,"a");
    if(log!=nullptr &&err!=nullptr)
    {
        FILE* cur;
        if(level==DEBUG || level==NORMAL|| level==WARNING)
            cur=log;
        if(level==ERROR ||level==FATAL)
            cur=err;
        fprintf(cur,"%s%s\n",logpre,logcontext);
    }
    fclose(log);
    fclose(err);
}