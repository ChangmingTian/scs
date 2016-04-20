
/*[2016-04-19T10:18:30]*/
/*永远都不要为了目的而忘了初衷。*/
/*就像给风命名的,不是它要去的方向,而是它来时的方向。*/
/*Email:372909853@qq.com*/
/*review by tcm*/

#ifndef __DEBUGA_H__
#define __DEBUGA_H__

#include <stdio.h>

/*NOTICE!!! debugf debugp debugf
 *是多任务安全的,使用了建议文件锁的阻塞模式
 *是在信号处理函数中不应该使用的
 *是不可重入的(使用了printf和文件锁)
 */

/*如果期望通过这里的宏控制所有的源文件
 *调试信息输出,需要修改当前头文件下面两行宏.
 *如果期望控制某个源文件中是否打印调试信息
 *则不能打开下面两行宏定义,需要在引用当前头
 *文件前定义这些宏*/
//#define DEBUGA_PRINTF_DEBUG //enable printf
//#define DEBUGA_FILE_DEBUG //enable fprintf

#ifndef DEBUGA_MAX_FILELEN 
	/*定义日志文件的最大字节数
	 *也可以在引用当前头文件前自行定义
         *当前日志文件满后,当前日志被重命名添加后缀.old*/
	#define DEBUGA_MAX_FILELEN (512*1024)
#endif
#ifndef DEBUGA_FILE_PATH 
	/*定义日志文件的存储位置
	 *也可以在引用当前头文件前自行定义*/
	#define DEBUGA_FILE_PATH "/log"
#endif
#ifndef DEBUGA_DEBUG_HEADER
	/*该信息会作为每条信息的开头在时间之后打印出来
	 *也可以在引用当前头文件前自行定义*/
	#define DEBUGA_DEBUG_HEADER "{debuga}"
#endif


void WriteStringToFile(char *filepath, char *head, char *stringformat,...);

#ifdef DEBUGA_PRINTF_DEBUG
    #define DEBUGP(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUGP(format, ...)
#endif

#ifdef DEBUGA_FILE_DEBUG
    #define DEBUGF(filepath, head, stringformat, ...)	WriteStringToFile(filepath,head,stringformat,##__VA_ARGS__)
#else
    #define DEBUGF(filepath, head, stringformat, ...)
#endif

#define ADDSTRING(x,y) x""y 

/*将日志信息输入到文件,每条信息末尾会自动添加回车
 *格式为：[DataTime] [DEBUGA_DEBUG_HEADER] [message]*/
#define debugf(sformat, ...)	DEBUGF(DEBUGA_FILE_PATH, DEBUGA_DEBUG_HEADER, sformat, ##__VA_ARGS__)

/*将日志信息输入到文件,每条信息末尾会自动添加回车
 *格式为：[DEBUGA_DEBUG_HEADER] [message]*/
#define debugp(sformat, ...)	DEBUGP(DEBUGA_DEBUG_HEADER); DEBUGP(sformat, ##__VA_ARGS__); DEBUGP("\n")
/*依次调用了debugf debugp*/
#define debuga(sformat, ...)	debugf(sformat, ##__VA_ARGS__); debugp(sformat, ##__VA_ARGS__)

#endif

