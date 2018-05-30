#pragma once

#include "pcb.h"

typedef list<PCB*> PCBLIST;

class PcbQueueTable
{
private:
	const processList listType;
	PCBLIST pcbList[3];

public:
	PcbQueueTable(processList listType):listType(listType) {}

public:
	void PushBack(PCB *pcb);
	PCB* PopFront();
	void Delete(PCB *pcb);
	
};