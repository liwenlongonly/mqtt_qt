//
// Created by ilong on 2022/3/23.
//

#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#ifndef LOGLEVEL
#define LOGLEVEL DEBUG
#endif

// 使用了GNU C扩展语法，只在gcc（C语言）生效，
// g++的c++版本编译不通过
static const char* s_loginfo[] = {
        [ERROR] = "ERROR",
        [WARN]  = "WARN",
        [INFO]  = "INFO",
        [DEBUG] = "DEBUG",
};

static void get_timestamp(char *buffer)
{
    time_t t;
    struct tm *p;
    struct timeval tv;
    int len;
    int millsec;

    t = time(NULL);
    p = localtime(&t);

    gettimeofday(&tv, NULL);
    millsec = (int)(tv.tv_usec / 1000);

    /* 时间格式：[2011-11-15 12:47:34:888] */
    len = snprintf(buffer, 32, "[%04d-%02d-%02d %02d:%02d:%02d:%03d] ",
                   p->tm_year+1900, p->tm_mon+1,
                   p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, millsec);

    buffer[len] = '\0';
}

void mylog1(const char* filename, int line, enum LogLevel level, const char* fmt, ...)
{
    if(level > LOGLEVEL)
        return;

    va_list arg_list;
    char buf[1024];
    memset(buf, 0, 1024);
    va_start(arg_list, fmt);
    vsnprintf(buf, 1024, fmt, arg_list);
    char time[32] = {0};

    // 去掉*可能*存在的目录路径，只保留文件名
    const char* tmp = strrchr(filename, '/');
    if (!tmp) tmp = filename;
    else tmp++;
    get_timestamp(time);

    switch(level){
        case DEBUG:
            //绿色
            printf("\033[1;32m%s[%s] [%s:%d] %s\n\033[0m", time, s_loginfo[level], tmp, line, buf);
            break;
        case INFO:
            //蓝色
            printf("\033[1;34m%s[%s] [%s:%d] %s\n\033[0m", time, s_loginfo[level], tmp, line, buf);
            break;
        case ERROR:
            //红色
            printf("\033[1;31m%s[%s] [%s:%d] %s\n\033[0m", time, s_loginfo[level], tmp, line, buf);
            break;
        case WARN:
            //黄色
            printf("\033[1;33m%s[%s] [%s:%d] %s\n\033[0m", time, s_loginfo[level], tmp, line, buf);
            break;
    }
    va_end(arg_list);
}