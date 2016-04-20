#include "debuga.h"
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <string.h>
#include <unistd.h>


int GetDataTime(char *datetime)
{

	time_t now;
	int cur=0;
	struct tm *timenow;
	time(&now);
	timenow = localtime(&now);
	datetime[0]='[';
	cur++;
	cur += sprintf(datetime+cur,"%04d-%02d-%02dT%02d:%02d:%02d",timenow->tm_year+1900,timenow->tm_mon+1,timenow->tm_mday,timenow->tm_hour,timenow->tm_min,timenow->tm_sec);
	datetime[cur++] = ']';
	datetime[cur]='\0';
	return cur;
}

/**
 * @brief WriteStringToFile
 * @param filepath log file path
 * @param st string  will be write to file
 * @param format format of file eg. "w" "a".... abandon
 * @param head header will add to st
 */
void WriteStringToFile(char *filepath, char *head, char *stringformat,...)
{
	FILE *fp;
	unsigned long filesize = -1;      
	struct stat statbuff;  
	if(stat(filepath, &statbuff) < 0)
	{  
		if((fp=fopen(filepath,"w"))==NULL)
    		{
			return;
    		}
  	}
  	else
  	{  
		if(flock(fileno(fp),LOCK_EX))
		{
			return;
		}
       		filesize = statbuff.st_size;  
       		if(filesize >= (DEBUGA_MAX_FILELEN))
       		{
	 		int filepathlen = strlen(filepath);
	 		char *newfilepath = (char *)malloc(filepathlen+6);
	 		if(newfilepath == NULL)
	 		{
	    			return;
	 		}
	 		strcpy(newfilepath,filepath);
	 		newfilepath[filepathlen] = '.';
	 		newfilepath[filepathlen+1] = 'o';
	 		newfilepath[filepathlen+2] = 'l';
	 		newfilepath[filepathlen+3] = 'd';
	 		newfilepath[filepathlen+4] = '\0';
         		rename(filepath,newfilepath);
         		if((fp=fopen(filepath,"w"))==NULL)
         		{
            			return;
         		}
       		}
       		else
       		{
         		if((fp=fopen(filepath,"a"))==NULL)
         		{
            			return;
         		}
       		}
  	}  

  	char datetime[128];
  	datetime[0] = '\0';
  	if(GetDataTime(datetime))
  	{
      		fputs(datetime,fp);
  	}
  	fputs(head,fp);
  	va_list arg_ptr;
  	va_start(arg_ptr, stringformat);
  	vfprintf(fp, stringformat,arg_ptr);
  	va_end(arg_ptr);
  	fprintf(fp,"\n");
	if(flock(fileno(fp),LOCK_EX))
	{
		return;
	}
  	fclose(fp);
}
