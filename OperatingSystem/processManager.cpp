#include "processManager.h"

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
		CreateProcess("init", INIT);
}

// 进程创建
int ProcessManager::CreateProcess(const string pName, priorities priority)
{
	if (runningPcb == nullptr && priority != INIT) return 1;
	// 从内存中申请一个空白PCB，并初始化PCB
	PCB* pcb = new PCB(pName, priority, runningPcb);
	if (pcb->getPid() == MAX_PROCESS_NUM) return 2;
	cout << "[  CREATE  ] ";
	pcb->showBriefInfo();

	// 将新创建的process image 加入主进程表
	processTable.push_back(pcb);

	// 插入相应优先级的就绪队列尾部
	readyList.PushBack(pcb);

	return 0;
}

// 在进程表中找到PID的进程
pTable_iter ProcessManager::RetrieveProcess(unsigned int pid)
{
	pTable_iter iter;

	for (iter = processTable.begin(); iter != processTable.end(); iter++)
		if ((*iter)->getPid() == pid)
			return iter;		

	return iter;
}

// 操作系统终止进程的过程如下（撤销原语）：
//		根据被终止进程的标识符，检索PCB，从中读出该进程的状态。
//		若被终止进程处于执行状态，立即终止该进程的执行，将处理机资源分配给其他进程。
//		若该进程还有子进程，则应将其所有子进程终止。
//		将该进程所拥有的全部资源，或归还给其父进程或归还给操作系统。
//		将该PCB从所在队列（链表）中删除。
int ProcessManager::DeleteProcess(unsigned int pid)
{
	pTable_iter iter = RetrieveProcess(pid);
	if (iter == processTable.end()) return 2;

	PCB * pcb = (*iter);

	vector<PCB*> child = pcb->getChild();

	for (vector<PCB*>::iterator iter = child.begin(); iter != child.end(); iter++) 
		DeleteProcess((*iter)->getPid());

	if (pcb == runningPcb) {
		runningPcb = nullptr;
		Schedule();
	}
	else 
	{
		// 从就绪/阻塞队列中删除进程
		switch (pcb->getPList())
		{
		case READYLIST:
			readyList.Delete(pcb);
			break;
		default:
			break;
		};
	}

	pcb->RelResourceAll();

	// 从进程表中撤销进程
	processTable.erase(iter);
	delete(pcb);

	cout << "[  DELETE  ] Process, PID: " << pid << endl;

	return 0;
}

void ProcessManager::ShowAllProcess()
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

		if ((*iter)->getPPid() == MAX_PROCESS_NUM)	cout << setw(5) << "/";
		else cout << setw(5) << (*iter)->getPPid();

		cout << setw(10) << (*iter)->getPriorityS()
			<< setw(10) << (*iter)->getPTypeS()
			<< endl;
	}
	cout << endl;
}

void ProcessManager::ShowOneProcess(unsigned int pid)
{
	(*RetrieveProcess(pid))->ShowAllInfo();
}

// 进程调度
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

void ProcessManager::BlockProcess(RCB* waitRcb, int waitNum)
{
	runningPcb->WaitResource(waitRcb, waitNum);
	runningPcb = nullptr;
}

void ProcessManager::WakeUpProcess(PCB* pcb)
{

	if (pcb == nullptr) return;
	readyList.PushBack(pcb);
	pcb->WakeUp();
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

PCB * ProcessManager::GetRunningPcb()
{
	return runningPcb;
}