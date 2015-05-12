#ifndef _PUBLIC_FUNC_H_
#define _PUBLIC_FUNC_H_

#include <string>
#include <vector>
#include <stdio.h>      
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <stdlib.h>

using namespace std;

enum LOG_LEVEL
{
	ll_sys=0,
	ll_info=1
};

typedef struct _para_
{
	char* value;
	int len;
	_para_()
	{
		value="";
		len=0;
	}
}PARA,*lpPara;

void WriteLog(LOG_LEVEL log_level,const char *fm, ...)__attribute__((format(printf,2,3)));

void ParseString(const char* strLine,  PARA* pFields, PARA* pValue, int& count);

std::string formatstr( const char* fm,... );

void split(std::string& str,std::string sep,std::vector<std::string>& res);

string& replace_all(string& str, const string& old_value, const string& new_value);

bool mkdir_p(const char *sPathName);

#endif
