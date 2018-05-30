#include "RCB.h"

bool RCB::ridPool[MAX_RESOURCE_NUM];

int RCB::RequestRid()
{
	for (int i = 0; i < MAX_RESOURCE_NUM; i++)
		if (ridPool[i] == RESOURCE_USABLE) {
			ridPool[i] = RESOURCE_UNUSABLE;
			return i;
		}
	return -1;
}

bool RCB::ReleaseRid()
{
	if (ridPool[rid] == RESOURCE_UNUSABLE)
	{
		ridPool[rid] = RESOURCE_USABLE;
		return true;
	}
	else 
	{
		return false;
	}
}

// 功能：申请资源
// 参数：申请资源数量
// 返回值：
//      1 错误代码：申请数超过最大资源量，驳回申请
//     -1 正常代码：申请数大于可分配资源量，阻塞进程
//      0 正常代码：申请成功
int RCB::RequestRes(int reqNum)
{
	if (reqNum > totalNum) 
	{
		return ERROR;	
	}
	else 
	{
		if (reqNum <= availableNum)
		{
			availableNum -= reqNum;
			return OK;
		}
		else
		{
			return BLOCK;
		}
	}
}

bool RCB::ReleaseRes(int relNum)
{
	// TODO 有bug需要改，需要分配下去的资源，某进程归还的资源量不能超过自身的申请到的总量
	if (relNum + availableNum <= totalNum)
	{
		availableNum += relNum;
		return true;
	}
	else
	{
		return false;
	}
}

void RCB::pushBackWaitList(PCB * pcb)
{
	waitList.push_back(pcb);
}

PCB * RCB::PopFrontWaitList(void)
{
	PCB *pcb;
	pcb = waitList.front();
	waitList.pop_front();
	return pcb;
}

void RCB::DeleteFromWaitList(PCB * pcb)
{
	for (list<PCB*>::iterator iter = waitList.begin(); iter != waitList.end(); iter++) 
	{
		if ((*iter) == pcb)
		{
			waitList.erase(iter);
			return;
		}
	}
}

PCB * RCB::IsProcessReady()
{
	if (waitList.size() > 0) 
	{
		if (waitList.front()->getWaitNum(this) <= availableNum)
		{
			return PopFrontWaitList();
		}
	}
	return nullptr;
}

void RCB::ShowBriefInfo()
{
	std::cout << "*  RID: " << rid
		<< "  Total Num: " << totalNum
		<< "  Available: " << availableNum << " *\n";
}

void RCB::ShowAllInfo()
{
	std::cout << "************* RCB INFO *************"
		<< "\nRID:\t\t"		<< rid
		<< "\nTotal Num:\t" << totalNum
		<< "\nAvailable:\t" << availableNum;
	std::cout << "\n************************************\n";
}

int RCB::GetRid()
{
	return rid;
}

void RCB::SetTotalNum(int delta)
{
	totalNum += delta;
}

int RCB::GetTotalNum()
{
	return totalNum;
}

RCB::RCB(int totalNum)
{
	rid = RequestRid();
	this->totalNum = totalNum;
	this->availableNum = totalNum;
}


RCB::~RCB()
{
	ReleaseRid();
}
