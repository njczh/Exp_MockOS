#pragma once

#include <stdlib.h>
#include <string>
using std::string;
#include <list>
using std::list;
#include <iostream>

#define MAX_PCB 1024
#define USABLE		false
#define UNUSABLE	true

enum processType	{ READY, RUNNING, BLOCKED };
enum processList	{ READYLIST, BLOCKLIST };
enum priorities		{ INIT, USER, SYSTEM };

extern class PCB;

struct processStatus
{
	processType pType;
	processList pList;
};

struct creationTree
{
	PCB* parent;
	list<PCB*> child;
};

class PCB
{
public:
	unsigned int	getPid();
	unsigned int	getPPid();
	string			getPName();
	string			getPTypeS();
	processList		getPList();
	priorities		getPriority();
	string			getPriorityS();
	list<PCB*>		getChild();

public:
	void setPid(unsigned int pid);
	//void setPName(string newName);
	void setPStatus(processStatus newstatus);
	//void setPStatus(processType newType);
	//void setPStatus(processList newList);
	//void setParent(PCB* newParent);
	void addChild(PCB* newChild);
	void delChild(unsigned int pid);
	//bool setPriority(priorities newPriority);
	//void InitPcb(const string pName, priorities priority, PCB * parent);
	//void FreePcb();

public:
	void ShowAllInfo();
	void showOnelineInfo();

public:
	static unsigned int getPidFromPool();
	bool freePidIntoPool();

public:
	PCB(const string pName, priorities priority, PCB * parent);
	~PCB();

private:
	static bool pidPool[MAX_PCB];

private:
	unsigned int	pid;				// PID
	string			pName;				// Name
										// Open Files
										// Other Resources			 ( resource which is occupied )
	processStatus	pStatus;			// Status 
										//		- Type : ready / running / blocked
										//		- List : Ready list / Block list
	creationTree	cTree;				// Creation tree
										//		- Parent
										//		- Children
	priorities		priority;			// Priority £º0, 1, 2 ( init, user, system)

};

