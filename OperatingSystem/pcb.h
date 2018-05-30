#pragma once

#include <stdlib.h>
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <iostream>

#include "RCB.h"

#define MAX_PROCESS_NUM 1024
#define USABLE		false
#define UNUSABLE	true

enum processType	{ READY, RUNNING, BLOCKED };
enum processList	{ READYLIST, BLOCKLIST };
enum priorities		{ INIT, USER, SYSTEM };

extern class PCB;
extern class RCB;

struct Resource
{
	int		owned;
	int		wait;
	RCB *	rcb;

	Resource(int owned, int request, RCB* rcb) {
		this->owned = owned;
		this->wait = request;
		this->rcb = rcb;
	}
};

struct ProcessStatus
{
	processType pType;
	processList pList;
};

struct CreationTree
{
	PCB*			parent;
	vector<PCB*>	child;
};

class PCB
{
public:
	int				getPid();
	int				getPPid();
	string			getPName();
	string			getPTypeS();
	processList		getPList();
	priorities		getPriority();
	string			getPriorityS();
	vector<PCB*>	getChild();
	int				getWaitNum(RCB * rcb);

public:
	void	AddChild(PCB* newChild);
	void	DelChild(int pid);
	void	AcqResource(RCB *rcb, int num);
	int		RelResource(RCB *rcb, int num);
	void	RelResourceAll();
	void	WaitResource(RCB *rcb, int num);
	//void setParent(PCB* newParent);
	//bool setPriority(priorities newPriority);

public:
	void	Ready();		// 新建态 --加载-> 就绪态
	void	Run();			// 就绪态 --调度-> 运行态
	void	TimeOut();		// 运行态 --超时-> 就绪态
	void	Block();		// 运行态 --等待-> 阻塞态
	void	WakeUp();		// 阻塞态 --事件-> 就绪态

public:
	void	ShowAllInfo();
	void	showBriefInfo();

public:
	int		RequestPid();
	bool	ReleasePid();

public:
	PCB(const string pName, priorities priority, PCB * parent);
	~PCB();

private:
	static bool pidPool[MAX_PROCESS_NUM];

private:
	int	pid;							// PID
	string			pName;				// Name
	vector<Resource>Resources;			// Other Resources			 ( resource which is occupied )
	ProcessStatus	pStatus;			// Status 
										//		- Type : ready / running / blocked
										//		- List : Ready list / Block list
	CreationTree	cTree;				// Creation tree
										//		- Parent
										//		- Children
	priorities		priority;			// Priority ：0, 1, 2 ( init, user, system)

};

