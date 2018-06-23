#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
using namespace std;

#include "processManager.h"
#include "ResourceManager.h"
#include "stringTool.h"

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
	{ "init",	InitOsCmd			},	// 1 - init
	{ "cr"	,	CreateProcessCmd	},	// 2 - cr		<name> <priority>(= 1 or 2)	
	{ "de"	,	DeleteProcessCmd	},	// 3 - de		<name>
	{ "req"	,	RequestResourceCmd	},	// 4 - req		<resource name> <# of units>	
	{ "rel"	,	ReleaseResourceCmd	},	// 5 - rel		<resource name> <# of units>	
	{ "to"	,	TimeOutCmd			},	// 6 - to										
	{ "ps"	,	ProcessStatusCmd	},	// 7 - ps		// list all processes and their status 
	{ "rs"	,	ResourceStatusCmd	},	// 8 - rs		// list all resources and their status
	{ "help",	ShowHelpManCmd		}	// 9 - help
};										  

int main()
{
	cout << "Mock OS is initializing ..." << endl;

	processManager->CreateInitProcess();
	
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

/*****************************************************
// Initial OS
// format：-init
******************************************************/
void InitOsCmd(const vector<string>& argvs)
{
	switch (argvs.size())
	{
	case 1:
		processManager->CreateInitProcess();
		break;
	case 2:
		if (argvs[1] == "-r") 
		{
			processManager->DeleteProcess(0);
			processManager->CreateInitProcess();
		}
		else 
			cout << "参数：\"" << argvs[1] << "\" 不合法，请检查输入!" << endl;
		break;
	default:
		cout << "命令不合法，请检查输入！正确格式：init [-r]" << endl;
		break;
	}
}

/*****************************************************
// create process	
// format：-cr		<name> <priority>(= 1 or 2)	
******************************************************/
void CreateProcessCmd(const vector<string> &argvs)
{
	int result = 0;

	switch (argvs.size())
	{
	case 2:	// 优先级缺省，默认优先级为USER
		result = processManager->CreateProcess(argvs[1]);
		break;
	case 3:	
		try
		{
			unsigned int priority = stoi(argvs[2]);
			if (priority == USER || priority == SYSTEM)
				result = processManager->CreateProcess(argvs[1], (priorities)priority);
			else
				cout << "参数：\"" << argvs[2] << "\" 不合法（优先级仅允许设置为1或2），请检查输入!" << endl;
		}
		catch (const std::invalid_argument &)
		{
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

/*****************************************************
// delete process
// format：-de		<pid>	
******************************************************/
void DeleteProcessCmd(const vector<string> &argvs)
{
	int aSize = argvs.size();
	if (aSize >= 2)
	{
		for (int i = 1; i < aSize; i++)
		{
			try
			{
				switch (processManager->DeleteProcess(stoi(argvs[i])))
				{
				case 0:
					break;
				//case 1:
				//	cout << "无法删除init进程！" << endl;
				//	break;
				case 2:
					cout << "不存在PID为" << argvs[i] << "的进程，使用 ps 命令查看所有进程！" << endl;
					break;
				default:
					break;
				}
			}
			catch (const std::exception&)
			{
				cout << "参数：\"" << argvs[i] << "\" 不合法（pid仅允许为数字），请检查输入!" << endl;
			}
		}
	}
	else 
	{
		cout << "命令不合法，请检查输入！正确格式：de <pid>" << endl;
	}
}

/*****************************************************
// request resource
// format：-req <rid> <num of resource>
******************************************************/
void RequestResourceCmd(const vector<string> &argvs)
{
	int rid, reqNum;
	switch (argvs.size())
	{
	case 3:
		try {
			rid = stoi(argvs[1]);
		}
		catch (const std::exception&) {
			cout << "参数：\"" << argvs[1] << "\" 不合法（RID仅允许为数字），请检查输入!" << endl;
			break;
		}
		try {
			reqNum = stoi(argvs[2]);
		}
		catch (const std::exception&) {
			cout << "参数：\"" << argvs[2] << "\" 不合法（资源数仅允许为数字），请检查输入!" << endl;
			break;
		}
		if (!resourceManager->Request(processManager->GetRunningPcb(), rid, reqNum))
			processManager->BlockProcess(resourceManager->GetResourceRcb(rid), reqNum);
		break;

	default:
		cout << "命令不合法，请检查输入！正确格式：req <rid> <resNum>" << endl;
		break;
	}
}

/*****************************************************
// release Resource
// format：-rel <rid> <num of resource>
******************************************************/
void ReleaseResourceCmd(const vector<string> &argvs) 
{
	int rid, relNum;
	PCB * pcb;
	switch (argvs.size())
	{
	case 3:
		try {
			rid = stoi(argvs[1]);
		}
		catch (const std::exception&) {
			cout << "参数：\"" << argvs[1] << "\" 不合法（RID仅允许为数字），请检查输入!" << endl;
			break;
		}
		try {
			relNum = stoi(argvs[2]);
		}
		catch (const std::exception&) {
			cout << "参数：\"" << argvs[2] << "\" 不合法（资源数仅允许为数字），请检查输入!" << endl;
			break;
		}
		pcb = resourceManager->Release(processManager->GetRunningPcb(), rid, relNum);
		processManager->WakeUpProcess(pcb);
		break;

	default:
		cout << "命令不合法，请检查输入！正确格式：req <rid> <resNum>" << endl;
		break;
	}
}

/*****************************************************
// show process status
// format：-ps [<pid>]
******************************************************/
void ProcessStatusCmd(const vector<string> &argvs)
{
	switch (argvs.size())
	{
	case 1:
		processManager->ShowAllProcess();
		break;
	case 2:
		try
		{
			processManager->ShowOneProcess(stoi(argvs[1]));
		}
		catch (const std::exception&)
		{
			cout << "参数：\"" << argvs[1] << "\" 不合法（pid仅允许为数字），请检查输入!" << endl;
		}
		break;
	default:
		cout << "命令不合法，请检查输入！正确格式: ps [<pid>]" << endl;
		break;
	}
}

/*****************************************************
// show Resource status
// format：-rs [<rid>]
******************************************************/
void ResourceStatusCmd(const vector<string>& argvs)
{
	switch (argvs.size())
	{
	case 1:
		resourceManager->ShowAllResources();
		break;
	case 2:
		try
		{
			resourceManager->ShowOneResource(stoi(argvs[1]));
		}
		catch (const std::exception&) 
		{
			cout << "参数：\"" << argvs[1] << "\" 不合法（pid仅允许为数字），请检查输入!" << endl;
		}
		break;
	default:
		cout << "命令不合法，请检查输入！正确格式：rs [<rid>]" << endl;
		break;
	}
}

/*****************************************************
// time out
// format：-to <times>
******************************************************/
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

/*****************************************************
// Show help Manual
// format：-help
******************************************************/
void ShowHelpManCmd(const vector<string>& argvs)
{
	cout << "- init		[-r]\n"
		<< "- cr		<name> [<priority>(= 1 or 2)]	// create process\n"
		<< "- de		<pid> [<pid2> <pid3> ...]	// delete process\n"
		<< "- req		<resource name> <# of units>	// wait resource\n"
		<< "- rel		<resource name> <# of units>	// release resource\n"
		<< "- to		[<times>]			// time out\n"
		<< "- ps		[<pid>]				// list all processes and their status\n"
		<< "- rs		[<rid>]				// list all resources and their status\n";
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