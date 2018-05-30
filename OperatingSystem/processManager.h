#pragma once

#include <stdlib.h>
#include <iomanip>
#include <iostream>
using namespace std;

#include "pcb.h"
#include "RCB.h"
#include "processQueueTable.h"

typedef vector<PCB*>::iterator pTable_iter;

class ProcessManager
{
public: // C.R.U.D.
	void		CreateInitProcess();
	int			CreateProcess(const string pName, priorities priority = USER);
	pTable_iter RetrieveProcess(unsigned int pid);
	int			DeleteProcess(unsigned int pid);

	void		ShowAllProcess();
	void		ShowOneProcess(unsigned int pid);

	void		Schedule();
	void		BlockProcess(RCB* waitRcb, int waitNum);
	void		WakeUpProcess(PCB* pcb);

	int			getProcessNum();
	PCB *		GetRunningPcb();
	bool		isCpuFree();

public:
	static ProcessManager* GetInstance();

public:
	ProcessManager():readyList(READYLIST) {};
	~ProcessManager(){};

private:
	vector<PCB*>	processTable;				// 主进程表
	PCB *			runningPcb;					// 执行指针
	PcbQueueTable	readyList;					// 就绪队列
};

extern ProcessManager* processManager;


