#pragma once
#include <vector>
using std::vector;

#include "pcb.h"
#include "RCB.h"

typedef vector<RCB*>::iterator rcb_iter;

class ResourceManager
{
public:
	int		CreateNewResource(int totalNum);
	RCB *	GetResourceRcb(int rid);
	int		UpdateResourceTotal(int rid, int delta);
	void	DeleteResource(int rid);

	int		Request(PCB *pcb, int rid, int reqNum = 1);
	PCB *	Release(PCB *pcb, int rid, int relNum = 1);

	void	ShowAllResources();
	void	ShowOneResource(int rid);

public:
	static ResourceManager * GetInstance();

public:
	ResourceManager();
	~ResourceManager();

private:
	rcb_iter RetrieveResource(int rid);
	
private:
	vector<RCB*> rcbTable;
};

extern ResourceManager * resourceManager;

