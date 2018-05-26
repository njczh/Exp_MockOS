#include "processManagement.h"

ProcessManager* processManager = ProcessManager::GetInstance();

ProcessManager * ProcessManager::GetInstance()
{
	static ProcessManager instance;
	return &instance;
}

void ProcessManager::CreateInitProcess() {
	if (processTable.size() > 0)
		std::cout << "init进程已存在，无法创建！" << std::endl;
	else
		Create("init", INIT);
}

// 进程创建
int ProcessManager::Create(const string pName, priorities priority)
{
	if (runningPcb == nullptr && priority != INIT) return 1;
	// 从内存中申请一个空白PCB，并初始化PCB
	PCB* pcb = new PCB(pName, priority, runningPcb);
	if (pcb->getPid() == MAX_PCB) return 2;
	cout << "[  CREATE  ] ";
	pcb->showOnelineInfo();

	// 将新创建的process image 加入主进程表
	processTable.push_back(pcb);

	// 插入相应优先级的就绪队列尾部
	readyList.PushBack(pcb);

	return 0;
}

// 在进程表中找到PID的进程
pTable_iter ProcessManager::Retrieve(unsigned int pid)
{
	pTable_iter iter;

	for (iter = processTable.begin(); iter != processTable.end(); iter++)
		if ((*iter)->getPid() == pid)
			return iter;		

	return iter;
}

// 进程撤销
int ProcessManager::Delete(unsigned int pid)
{
	//if (pid == 0) return 1;

	pTable_iter iter = Retrieve(pid);

	if (iter == processTable.end()) return 2;

	PCB * pcb = (*iter);

	list<PCB*> child = pcb->getChild();

	for (list<PCB*>::iterator iter = child.begin(); iter != child.end(); iter++) {
		Delete((*iter)->getPid());
	}

	if (pcb == runningPcb) 
		runningPcb = nullptr;
	else {
		// 从就绪/阻塞队列中删除进程
		switch (pcb->getPList())
		{
		case READYLIST:
			readyList.Delete(pcb);
			break;
		case BLOCKLIST:
			blockedList.Delete(pcb);
			break;
		default:
			break;
		};
	}

	// 从进程表中撤销进程
	processTable.erase(iter);

	delete(pcb);

	cout << "[  DELETE  ] Process, PID: " << pid << endl;

	return 0;
}

void ProcessManager::ShowAllProcessByOneline()
{
	cout << left << setw(6) << "NAME"
		<< right << setw(5) << "PID"
		<< setw(5) << "PPID"
		<< setw(10) << "PRIORITY"
		<< setw(10) << "STAT"
		<< endl;
	for (pTable_iter iter = processTable.begin(); iter != processTable.end(); iter++) 
	{
		cout << left << setw(6) << (*iter)->getPName()
			<< right << setw(5) << (*iter)->getPid();

		if ((*iter)->getPPid() == MAX_PCB)	cout << setw(5) << "/";
		else cout << setw(5) << (*iter)->getPPid();

		cout << setw(10) << (*iter)->getPriorityS()
			<< setw(10) << (*iter)->getPTypeS()
			<< endl;
	}
	cout << endl;
}

void ProcessManager::ShowOneProcess(unsigned int pid)
{
	(*Retrieve(pid))->ShowAllInfo();
}

void ProcessManager::Schedule()
{
	readyList.PushBack(runningPcb);
	runningPcb = readyList.PopFront();

	if (isCpuFree())
		cout << "[ SCHEDULE ] Now NO process is running! Use 'to' to schedule process" << endl;
	else
		cout << "[ SCHEDULE ] Now Process: \" " << runningPcb->getPName()
		<< " (PID: " << runningPcb->getPid() << ") \" is running!" << endl;

}

int ProcessManager::getProcessNum()
{
	return processTable.size();
}

bool ProcessManager::isCpuFree()
{
	if (runningPcb == nullptr)
		return true;
	else
		return false;
}



// 进程调度
