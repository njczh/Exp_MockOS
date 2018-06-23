#include "pcb.h"

bool PCB::pidPool[MAX_PROCESS_NUM]; // false 表示可用，true 表示不可用

int PCB::RequestPid()
{
	for (int i = 0; i < MAX_PROCESS_NUM; i++) 
	{
		if (pidPool[i] == USABLE)
		{
			pidPool[i] = UNUSABLE;
			return i;
		}
	}
	return MAX_PROCESS_NUM;
}

bool PCB::ReleasePid()
{
	if (pidPool[pid] == UNUSABLE)
	{
		pidPool[pid] = USABLE;
		return true;
	}
	else
	{
		return false;
	}
}

/*************************************************************************************/

PCB::PCB(const string pName, priorities priority, PCB * parent)
{
	this->pid = RequestPid();

	// Process Name
	this->pName = pName;

	// Process Status
	pStatus.pType = READY;
	pStatus.pList = READYLIST;

	// Creation Tree
	this->cTree.parent = parent;
	if (parent != nullptr)
		parent->AddChild(this);

	// Priority
	this->priority = priority;
}

PCB::~PCB()
{
	ReleasePid();
	if (cTree.parent != nullptr)
		cTree.parent->DelChild(this->pid);
}

/*************************************************************************************/

int PCB::getPid()
{
	return pid;
}

int PCB::getPPid()
{
	if (cTree.parent != nullptr)
		return cTree.parent->getPid();
	else
		return MAX_PROCESS_NUM;
}

string PCB::getPName()
{
	return pName;
}

string PCB::getPTypeS()
{
	std::string type;
	switch (pStatus.pType) {
	case READY:
		type = "Ready"; break;
	case RUNNING:
		type = "Running"; break;
	case BLOCKED:
		type = "Blocked"; break;
	}
	return type;
}

processList PCB::getPList()
{
	return pStatus.pList;
}

priorities PCB::getPriority()
{
	return priority;
}

string PCB::getPriorityS()
{
	string s_priority;
	switch (priority)
	{
	case INIT:
		s_priority = "Init";
		break;
	case USER:
		s_priority = "User";
		break;
	case SYSTEM:
		s_priority = "System";
		break;
	}
	return s_priority;
}

vector<PCB*> PCB::getChild()
{
	return cTree.child;
}

int PCB::getWaitNum(RCB * rcb)
{
	for (vector<Resource>::iterator iter = Resources.begin(); iter != Resources.end(); iter++)
	{
		if (iter->rcb == rcb)
		{
			return iter->wait;
		}
	}
	return -1;
}

/*************************************************************************************/

void PCB::AddChild(PCB * newChild)
{
	cTree.child.push_back(newChild);
}

void PCB::DelChild(int pid) {
	for (vector<PCB*>::iterator iter = cTree.child.begin(); iter != cTree.child.end(); iter++) {
		if ((*iter)->getPid() == pid) {
			cTree.child.erase(iter);
			return;
		}
	}	
}

void PCB::AcqResource(RCB * rcb, int acqNum)
{
	for (vector<Resource>::iterator iter = Resources.begin(); iter != Resources.end(); iter++)
	{
		if (iter->rcb == rcb)
		{
			if (iter->wait >= acqNum) 
				iter->wait -= acqNum; 
			iter->owned += acqNum;
			return;
		}
	}

	Resource newResource(acqNum, 0, rcb);
	Resources.push_back(newResource);
}

int PCB::RelResource(RCB * rcb, int relNum)
{
	for (vector<Resource>::iterator iter = Resources.begin(); iter != Resources.end(); iter++)
	{
		if (iter->rcb == rcb)
		{
			if (iter->owned >= relNum)
			{
				iter->owned -= relNum;
				if (iter->owned == 0)
					Resources.erase(iter);
				return 0;
			}
			else
				return 1;
		}
	}
	return 2;
}

void PCB::RelResourceAll()
{
	for (vector<Resource>::iterator iter = Resources.begin(); iter != Resources.end(); iter++)
	{
		if (iter->wait != 0)
			iter->rcb->DeleteFromWaitList(this);
		iter->rcb->ReleaseRes(iter->owned);
	}
}

/***********************************************************************************
* #1 FIX:	2018/06/23 23:01	修复已经拥有资源后在挂起等待出现进程状态没有修改的bug 
***********************************************************************************/
void PCB::WaitResource(RCB * rcb, int waitNum)
{
	Block();	// 改变进程状态，变为阻塞等待

	for (vector<Resource>::iterator iter = Resources.begin(); iter != Resources.end(); iter++)
	{
		if (iter->rcb == rcb)
		{
			iter->wait += waitNum;
			return;
		}
	}

	// 如果没有拥有该资源则保存该资源的拥有（等待）情况
	Resource newResource(0, waitNum, rcb);
	Resources.push_back(newResource);

}

/*************************************************************************************/

// 新建态 --加载-> 就绪态
void PCB::Ready()
{
	pStatus.pType = READY;
	pStatus.pList = READYLIST;
}

// 就绪态 --调度-> 运行态
void PCB::Run()
{
	pStatus.pType = RUNNING;
}

// 运行态 --超时-> 就绪态
void PCB::TimeOut()
{
	pStatus.pType = READY;
	pStatus.pList = READYLIST;
}

// 运行态 --等待-> 阻塞态
void PCB::Block()
{
	pStatus.pType = BLOCKED;
	pStatus.pList = BLOCKLIST;
}

// 阻塞态 --事件-> 就绪态
void PCB::WakeUp()
{
	pStatus.pType = READY;
	pStatus.pList = READYLIST;
}

/*************************************************************************************/

void PCB::ShowAllInfo()
{
	std::cout << "************* PCB INFO *************"
		<< "\nPID:\t\t" << pid
		<< "\nName:\t\t" << pName
		<< "\nPriority:\t" << getPriorityS();

	std::string status = getPTypeS();

	if (pStatus.pType != RUNNING) {
		switch (pStatus.pList) {
		case READYLIST:
			status += "\n       List:\tReady List"; break;
		case BLOCKLIST:
			status += "\n       List:\tBlocked List"; break;
		}
	}

	std::cout << "\nStatus Type:\t" << status;

	if (cTree.parent != NULL)
		std::cout << "\nParent PID:\t" << cTree.parent->getPid();
	else
		std::cout << "\nParent PID:\tNULL";

	if (cTree.child.size() != 0) 
	{
		std::cout << "\nChild PID:\t";
		for (const auto &child : cTree.child)
			std::cout << child->getPid() << " ";
	}
	else
		std::cout << "\nChild PID:\tNULL";

	if (Resources.size() != 0) 
	{
		std::cout << "\nResource:";
		for (vector<Resource>::iterator iter = Resources.begin(); iter != Resources.end(); iter++) 
		{
			std::cout << "\n    RID: " << iter->rcb->GetRid()
				<< "    owned: " << iter->owned
				<< "    wait: " << iter->wait;
		}
	}
	else
		std::cout << "\nResource:\tNULL";

	std::cout << "\n************************************\n";

}

void PCB::showBriefInfo()
{
	std::cout << getPriorityS() <<" Process, Name: " << pName
		<< ", PID: " << pid << "\n";
}

/*************************************************************************************/