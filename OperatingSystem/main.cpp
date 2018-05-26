#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

#include "processManagement.h"
#include "stringTool.h"

/************************************************************
Mandatory Commands
1 -init
2 -cr		<name> <priority>(= 1 or 2)		// create process
3 -de		<name>							// delete process
4 -req		<resource name> <# of units>	// request resource
5 -rel		<resource name> <# of units>	// release resource
6 -to										// time out
Optional Commands
7 -list all processes and their status -ps
8 -list all resources and their status
9 -provide information about a given process
*************************************************************/

void InitOsCmd(const vector<string> &argvs);
void CreateProcessCmd(const vector<string> &argvs);
void DeleteProcessCmd(const vector<string> &argvs);
void RequestResourceCmd(const vector<string> &argvs);
void ReleaseResourceCmd(const vector<string> &argvs);
void ProcessStatusCmd(const vector<string> &argvs);
void ResourceStatusCmd(const vector<string> &argvs);
void TimeOutCmd(const vector<string> &argvs);
void ShowHelpManCmd(const vector<string> &argvs);

void getArgvs(vector<string> &argvs);

static const map<string, void(*)(const vector<string> &)> mapCommands = {
	{ "init",	InitOsCmd			},
	{ "cr"	,	CreateProcessCmd	},
	{ "de"	,	DeleteProcessCmd	},
	{ "req"	,	RequestResourceCmd	},
	{ "rel"	,	ReleaseResourceCmd	},
	{ "to"	,	TimeOutCmd			},
	{ "ps"	,	ProcessStatusCmd	},
	{ "rs"	,	ResourceStatusCmd	},
	{ "help",	ShowHelpManCmd		}
};

int main()
{
	cout << "Mock OS is initializing ..." << endl;

	processManager->CreateInitProcess();
	processManager->Schedule();
	
	// 运行test shell读入用户命令
	cout << "\n===================================== ";
	cout << "Test Shell";
	cout << " =====================================" << endl;
	ShowHelpManCmd(vector<string>());
	while (processManager->getProcessNum() != 0)
	{
		vector<string> argvs;
		getArgvs(argvs);
			
		try{
			mapCommands.at(argvs[0])(argvs);
		}
		catch (const std::exception&){
			cout  << "命令：\""<< argvs[0]<<"\" 无效，请检查输入!" << endl;
		}		
	}
}

// Initial OS
// format：-init
void InitOsCmd(const vector<string>& argvs)
{
	switch (argvs.size())
	{
	case 1:
		processManager->CreateInitProcess();
		break;
	case 2:
		if (argvs[1] == "-r") {
			processManager->Delete(0);
			processManager->CreateInitProcess();
		}
		else {
			cout << "参数：\"" << argvs[1] << "\" 不合法，请检查输入!" << endl;
		}
		break;
	default:
		cout << "命令不合法，请检查输入！正确格式：init [-r]" << endl;
		break;

	}
}

// create process	
// format：-cr		<name> <priority>(= 1 or 2)		
void CreateProcessCmd(const vector<string> &argvs)
{
	int result = 0;
	switch (argvs.size())
	{
	case 2:
		result = processManager->Create(argvs[1]);
		break;
	case 3:
		try
		{
			unsigned int priority = stoi(argvs[2]);
			if (priority == USER || priority == SYSTEM)
				result = processManager->Create(argvs[1], (priorities)priority);
			else
				cout << "参数：\"" << argvs[2] << "\" 不合法（优先级仅允许设置为1或2），请检查输入!" << endl;
		}catch (const std::invalid_argument &){
			cout << "参数：\"" << argvs[2] << "\" 不合法（优先级仅允许为数字：1或2），请检查输入!" << endl;
		}
		break;
	default:
		cout << "命令不合法，请检查输入！正确格式：cr <name> [<priority>(= 1 or 2, default = 1)]" << endl;
		break;
	}	
	switch (result)
	{
	case 1:
		cout << "当前无进程执行，无法创建进程！使用 to 命令进行进程调度！" << endl;
		break;
	case 2:
		cout << "进程创建已达上限，无法继续创建!" << endl;
	default:
		break;
	}
}

// delete process
// format：-de		<pid>	
void DeleteProcessCmd(const vector<string> &argvs)
{
	int aSize = argvs.size();
	if (aSize >= 2) {
		for (int i = 1; i < aSize; i++) {
			try{
				unsigned int pid = stoi(argvs[i]);
				switch (processManager->Delete(pid)) {
				case 0:
					break;
				case 1:
					cout << "无法删除init进程！" << endl;
					break;
				case 2:
					cout << "不存在PID为" << pid << "的进程，使用 ps 命令查看所有进程！" << endl;
					break;
				default:
					break;
				}
			}catch (const std::exception&){
				cout << "参数：\"" << argvs[i] << "\" 不合法（pid仅允许为数字），请检查输入!" << endl;
			}
		}
	}else
		cout << "命令不合法，请检查输入！正确格式：de <pid>" << endl;

}

void RequestResourceCmd(const vector<string> &argvs) {
	cout << "敬请期待" << endl;
}

void ReleaseResourceCmd(const vector<string> &argvs) {
	cout << "敬请期待" << endl;
}

void ProcessStatusCmd(const vector<string> &argvs)
{
	switch (argvs.size())
	{
	case 1:
		processManager->ShowAllProcessByOneline();
		break;
	case 2:
		try
		{
			processManager->ShowOneProcess(stoi(argvs[1]));
		}catch (const std::exception&){
			cout << "参数：\"" << argvs[1] << "\" 不合法（pid仅允许为数字），请检查输入!" << endl;
		}
		break;
	default:
		cout << "命令不合法，请检查输入！正确格式: ps [<pid>]" << endl;
		break;
	}
}

void ResourceStatusCmd(const vector<string>& argvs)
{
	cout << "敬请期待" << endl;
}

// time out
// format：-to <times>
void TimeOutCmd(const vector<string> &argvs)
{
	int times = 0;

	switch(argvs.size())
	{
	case 1:
		times = 1;
		break;
	case 2:
		try{
			times = stoi(argvs[1]);
		}catch (const std::exception&) {
			cout << "参数：\"" << argvs[1] << "\" 不合法（超时轮转次数仅允许为数字），请检查输入!" << endl;
		}
		break;
	default:
		cout << "命令不合法，请检查输入！正确格式：to <times>" << endl;
		break;
	}

	for (int i = 1; i <= times; i++) {
		processManager->Schedule();
	}
}

// Show help Manual
// format：-help
void ShowHelpManCmd(const vector<string>& argvs)
{
	cout << "- init		[-r]\n"
		<< "- cr		<name> [<priority>(= 1 or 2)]	// create process\n"
		<< "- de		<pid> [<pid2> <pid3> ...]	// delete process\n"
		<< "- req		<resource name> <# of units>	// request resource\n"
		<< "- rel		<resource name> <# of units>	// release resource\n"
		<< "- to		[<times>]			// time out\n"
		<< "- ps		[<pid>]				// list all processes and their status\n"
		<< "- rs						// list all resources and their status\n";
}

void getArgvs(vector<string> &argvs) 
{
	while (argvs.size() == 0) 
	{
		cout << "shell>";
		string cmd;
		getline(cin, cmd);
		split(cmd, ' ', argvs);
	}
}