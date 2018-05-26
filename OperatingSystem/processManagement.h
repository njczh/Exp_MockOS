#pragma once

#include <stdlib.h>
#include <iomanip>
#include <iostream>
using namespace std;

#include "pcb.h"
#include "processQueueTable.h"

typedef list<PCB*>::iterator pTable_iter;

class ProcessManager
{
public: // C.R.U.D.
	void		CreateInitProcess();
	int			Create(const string pName, priorities priority = USER);
	pTable_iter Retrieve(unsigned int pid);
	int			Delete(unsigned int pid);
	void		ShowAllProcessByOneline();
	void		ShowOneProcess(unsigned int pid);
	void		Schedule();
	int			getProcessNum();
	bool		isCpuFree();

public:
	static ProcessManager* GetInstance();
	ProcessManager():readyList(READYLIST),blockedList(BLOCKLIST) {};
	~ProcessManager() {};

private:
	std::list<PCB*> processTable;				// 主进程表

	PCB* runningPcb;							// 执行指针
	PcbQueueTable readyList;						// 就绪队列
	PcbQueueTable blockedList;					// 阻塞队列
};

extern ProcessManager* processManager;


