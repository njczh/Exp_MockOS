#pragma once
#include <list>
using std::list;

#include "pcb.h"

#define MAX_RESOURCE_NUM 1024
#define RESOURCE_USABLE false
#define RESOURCE_UNUSABLE true

#define ERROR 1
#define BLOCK -1
#define OK 0

extern class PCB;

class RCB
{
public:
	int		RequestRid(void);
	bool	ReleaseRid();

	int		RequestRes(int reqNum = 1);
	bool	ReleaseRes(int relNum = 1);

	void	pushBackWaitList(PCB *pcb);
	PCB*	PopFrontWaitList(void);
	void	DeleteFromWaitList(PCB *pcb);
	PCB*	IsProcessReady();

	void	ShowBriefInfo();
	void	ShowAllInfo();

public:
	int		GetRid();
	void	SetTotalNum(int delta);
	int		GetTotalNum();
	// int		GetAvailableNum();

public:
	RCB(int totalNum);
	~RCB();

private:
	static bool ridPool[MAX_RESOURCE_NUM];

private:
	// string		rName;
	int			rid;
	int			totalNum;
	int			availableNum;
	list<PCB*>	waitList;
};

