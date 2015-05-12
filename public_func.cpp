#include "public_func.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <stdarg.h>
#include <inttypes.h>
#include <string.h>
#include <iconv.h>
#include <string>

static volatile LOG_LEVEL s_log_level=ll_sys;

void WriteLog(LOG_LEVEL log_level,const char *fm,...)
{
	FILE *fp;
	char FileName[128];
	time_t ut;
	struct tm lt;
	va_list vl;

	//if(log_level>s_log_level)
	//	return;

	mkdir("/data/log", 0755)
	mkdir("/data/log/listen_app", 0755);

	time(&ut);
	localtime_r(&ut,&lt);

	sprintf(FileName,"/data/log/listen_app/listen_app_%04u%02u%02u.log",lt.tm_year+1900,lt.tm_mon+1,lt.tm_mday);
	fp=fopen(FileName,"a");
	if(fp==NULL)
		return;

	fprintf(fp,"%04u-%02u-%02u %02u:%02u:%02u ",lt.tm_year+1900,lt.tm_mon+1,lt.tm_mday,lt.tm_hour,lt.tm_min,lt.tm_sec);

	va_start(vl,fm);

	vfprintf(fp,fm,vl);
	fprintf(fp,"\n");

	va_end(vl);

	fclose(fp);
}

void ParseString(const char* strLine, PARA *pField, PARA *pValue, int& count)
{
	char *tp = (char*)strLine;
	char *e="\r\n";
	int arraycount=100;
	count=0;

	uint32_t len=strlen(tp);
	int splitword=strlen(e);

	char * p = (char *)strstr(strLine,e);

	char * q;
	int len2;
	while(p!=NULL)
	{
		q=NULL;
		*p = 0;
		int off=p-tp;

		len2=off;
		q = (char *)strstr(tp,"=");
		if(q!=NULL)
		{
			*q = 0;
			int off1=q-tp;
			pField[count].value=tp;
			pField[count].len=off1;
			tp=q+1;
			len2-=(off1+1);

			if( len2 > 0 )
			{
				pValue[count].value=tp;
				pValue[count].len=len2;
			}
			count++;
		}

		if(count>=arraycount)
			return ;
		tp=p+splitword;
		len-=(off+splitword);
		p=strstr(tp,e);
	}
	if(len>0)
	{
		q=strstr(tp,"=");
		if(q!=NULL)
		{
			*q = 0;
			int off1=q-tp;
			pField[count].value=tp;
			pField[count].len=off1;
			tp=q+1;
			len-=(off1+1);

			if( len > 0 )
			{
				pValue[count].value=tp;
				pValue[count].len=len;
				count++;
			}
		}
	}

	return;
}

std::string formatstr(const char* fm,...)
{
	int iSize=0;
	char buff[1024*16];

	memset(buff, 0, sizeof(buff));

	va_list args;
	va_start( args, fm );
	iSize = vsnprintf( buff, sizeof(buff) - 1, fm, args );
	va_end( args );

	return std::string(buff);
}

void split(std::string& str, std::string sep,std::vector<std::string>& res)
{
	size_t last = 0;
	size_t index = str.find_first_of(sep, last);
	while (index!=std::string::npos)
	{
		std::string tmp = str.substr(last,index-last);
		if (tmp.length() > 0)
		{
			res.push_back(tmp);
		}		
		last=index+1;
		index=str.find_first_of(sep,last);
	}
	if (index-last > 0)
	{
		std::string tmp = str.substr(last,index-last);
		if (tmp.length() > 0)
		{
			res.push_back(tmp);
		}		
	}
}

string& replace_all(string& str, const string& old_value, const string& new_value)
{     
	while(true)
	{
		string::size_type pos(0);
		if((pos = str.find(old_value)) != string::npos)
		{
			str.replace(pos, old_value.length(), new_value);
		}
		else
		{
			break;
		}
	}  
	return str;
}

bool mkdir_p(const char *sPathName)
{
	char   DirName[256];
	strcpy(DirName,   sPathName);
	int   i,len   =   strlen(DirName);
	if(DirName[len-1]!='/')
		strcat(DirName,"/");

	len   =   strlen(DirName);
	for(i=1;   i<len;   i++)
	{
		if(DirName[i]=='/')
		{
			DirName[i]   =   0;
			if( access(DirName,   0) !=0  )
			{
				if(mkdir(DirName,0755)==-1)
				{
					perror("mkdir   error");
					return   false;
				}
			}
			DirName[i] = '/';
		}
	}
	return  true;
} 
