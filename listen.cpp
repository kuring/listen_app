#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <vector>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>

#include "listen.h"
#include "public_func.h"

void Listen::add(std::string app)
{
	// 将map中的value初始化为-1，-1表示进程未启动, 0表示子进程以daemon方式运行，大于0表示进程号
	_apps.insert(std::pair<std::string, pid_t>(app, -1));
}

size_t Listen::get_apps_size()
{
	return _apps.size();
}

void Listen::run_app(std::string app)
{
	WriteLog(ll_info, "start to run process %s", app.c_str());
	pid_t pid = fork();
	if (pid == 0)
	{
		// child process
		WriteLog(ll_info, "child process %s running", app.c_str());
		execl("/bin/sh", "sh", "-c", app.c_str(), NULL);
		WriteLog(ll_info, "child process %s exit, errno = %d %m", app.c_str(), errno);
		_exit(1);
	}
	else if (pid > 0)
	{
		// parent process
		_apps[app] = pid;
	}
}

void Listen::start()
{
	// run all process
	for (std::map<std::string, pid_t>::iterator iter = _apps.begin(); iter != _apps.end(); iter++)
	{
		run_app(iter->first);
	}

	// check process
	while (1)
	{
		for (std::map<std::string, pid_t>::iterator iter = _apps.begin(); iter != _apps.end(); iter++)
		{
			if (iter->second > 0)
			{
				pid_t result = waitpid(iter->second, NULL, WNOHANG);
				if (result == iter->second)
				{
					// child exit				
					// check the child process is still running by deamon or already running before this process running
					std::vector<pid_t> pids;
					find_pid_by_name(iter->first.c_str(), pids);
					if (pids.size() > 0)
					{
						// the child process is still running by deamon or already running before this process running
						_apps[iter->first] = 0;
					}
					else
					{
						// child exit and not running by daemon, then run child process
						_apps[iter->first] = -1;
						run_app(iter->first);
					}					
				}
			}
			else if (iter->second == 0)
			{
				// child process running by deamon, check process is running
				std::vector<pid_t> pids;
				find_pid_by_name(iter->first.c_str(), pids);
				if (pids.size() == 0)
				{
					// the process is not running, run the process
					run_app(iter->first);
				}
			}
			else
			{
				run_app(iter->first);
			}
		}
		sleep(1);
	}
}

int Listen::find_pid_by_name(const char *name, std::vector<pid_t> &pids)
{
	DIR *dir;
	struct dirent *d;
	int pid;

	/* Open the /proc directory. */
	dir = opendir("/proc");
	if (!dir)
	{
		printf("cannot open /proc");
		return -1;
	}

	/* Walk through the directory. */
	while ((d = readdir(dir)) != NULL)
	{
		char cmdline[PATH_MAX+1];
		char path[PATH_MAX+1];
		int len;

		/* See if this is a process */
		if ((pid = atoi(d->d_name)) == 0)
			continue;

		snprintf(cmdline, sizeof(cmdline), "/proc/%s/cmdline", d->d_name);
		FILE *fp = fopen(cmdline, "rb");
		if (!fp)
		{
			return -1;
		}
		char cmd[PATH_MAX+1];
		int size = fread(cmd, 1, PATH_MAX, fp);
		fclose(fp);
		for (int i=0; i<size - 1; i++)
		{
			if (cmd[i] == '\0')
			{
				cmd[i] = ' ';
			}
		}
		if (strcmp(cmd, name) == 0)
		{
			pids.push_back(pid);
		}
	}
	closedir(dir);
	return 0;
}

int Listen::killall()
{
	if (_apps.size() == 0)
	{
		return -1;
	}
	
	for (std::map<std::string, pid_t>::iterator iter = _apps.begin(); iter != _apps.end(); iter++)
	{
		std::vector<pid_t> pids;
		find_pid_by_name(iter->first.c_str(), pids);
		for (std::vector<pid_t>::iterator iter2 = pids.begin(); iter2 != pids.end(); iter2++)
		{			
			WriteLog(ll_info, "kill process %s ,pid=%d\n", iter->first.c_str(), *iter2);
			kill(*iter2, 9);
		}
	}
	return 1;
}
