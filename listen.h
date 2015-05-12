#ifndef _LISTEN_H_
#define _LISTEN_H_

#include <unistd.h>
#include <map>
#include <string>

class Listen
{
public:
	void add(std::string app);

	/**
	 * 采用循环遍历子进程并调用非阻塞waitpid系统调用的方式来查找子进程是否存在
	 */
	void start();
	
	/**
	 * 获取需要监控的进程数
	 */
	size_t get_apps_size();
	
	/**
	 * 将监控的进程名杀死，一般用于该程序刚启动时先将要监控的杀死后便于用waitpid的方式监控
	 */
	int killall();

private:
	void run_app(std::string app);
	
	/**
	 * get process id by process name
	 */
	int find_pid_by_name(const char *ProcName, std::vector<pid_t> &pids);

private:
	std::map<std::string, pid_t> _apps;
};

#endif
