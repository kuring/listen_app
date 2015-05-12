#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <string>

#include "public_func.h"
#include "listen.h"

static const std::string ini_file = "listen_app.ini";

int main(int argc, char *argv[])
{
	printf("progress build %s %s\n", __DATE__, __TIME__);
	
	Listen listen;
	// read ini file
	std::ifstream in(ini_file.c_str(), ios::in);
	if (in)
	{
		std::string line;
		while (std::getline(in, line))
		{
			line.erase(0, line.find_first_not_of(' '));
			line.erase(line.find_last_not_of(' ') + 1);
			line.erase(line.find_last_not_of('\r') + 1);
			if (line.length() > 0)
			{
				listen.add(line);
			}
		}
	}
	in.close();
	
	if (listen.get_apps_size() == 0)
	{
		printf("read file %s error\n", ini_file.c_str());
		_exit(-1);
	}
	listen.killall();
	
	printf("progress change to daemon, pid=%d\n", getpid());
	daemon(0, 0);
	listen.start();
	
	return 1;
}
