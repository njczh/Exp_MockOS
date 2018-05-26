#include "pcb.h"
#include <iostream>


bool PCB::pidPool[MAX_PCB]; // false 表示可用，true 表示不可用

unsigned int PCB::getPidFromPool()
{
	for (unsigned int i = 0; i < MAX_PCB; i++) {
		if (pidPool[i] == USABLE) {
			pidPool[i] = UNUSABLE;
			return i;
		}
	}
	return MAX_PCB;
}

bool PCB::freePidIntoPool()
{
	if (pid < MAX_PCB) {
		pidPool[pid] = USABLE;
		return true;
	}
	else
		return false;
}

/*************************************************************************************/

PCB::PCB(const string pName, priorities priority, PCB * parent)
{
	this->pid = getPidFromPool();

	// Process Name
	this->pName = pName;

	// Process Status
	pStatus.pType = READY;
	pStatus.pList = READYLIST;

	// Creation Tree
	this->cTree.parent = parent;
	if (parent != nullptr)
		parent->addChild(this);

	// Priority
	this->priority = priority;
}

PCB::~PCB()
{
	freePidIntoPool();
	if (cTree.parent != nullptr)
		cTree.parent->delChild(this->pid);
}

/*************************************************************************************/

unsigned int PCB::getPid()
{
	return pid;
}

unsigned int PCB::getPPid()
{
	if (cTree.parent != nullptr)
		return cTree.parent->getPid();
	else
		return MAX_PCB;
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
	switch (priority) {
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

list<PCB*> PCB::getChild()
{
	return cTree.child;
}

/*************************************************************************************/

void PCB::setPid(unsigned int pid)
{
	this->pid = pid;
}

void PCB::setPStatus(processStatus newstatus)
{
	this->pStatus = newstatus;
}

void PCB::addChild(PCB * newChild)
{
	cTree.child.push_back(newChild);
}

void PCB::delChild(unsigned int pid) {
	for (list<PCB*>::iterator iter = cTree.child.begin(); iter != cTree.child.end(); iter++) {
		if ((*iter)->getPid() == pid) {
			cTree.child.erase(iter);
			return;
		}
	}	
}

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

	std::cout << "\n************************************\n";

}

void PCB::showOnelineInfo()
{
	std::cout << getPriorityS() <<" Process, Name: " << pName
		<< ", PID: " << pid << "\n";
}

/*************************************************************************************/