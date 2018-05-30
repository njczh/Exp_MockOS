#include "ResourceManager.h"

ResourceManager * resourceManager = ResourceManager::GetInstance();

int ResourceManager::CreateNewResource(int totalNum)
{
	RCB *rcb = new RCB(totalNum);
	rcbTable.push_back(rcb);
	std::cout << "新资源" << rcb->GetRid() << "创建\n";
	return rcb->GetRid();
}

rcb_iter ResourceManager::RetrieveResource(int rid)
{
	rcb_iter iter;

	for (iter = rcbTable.begin(); iter != rcbTable.end(); iter++) 
		if ((*iter)->GetRid() == rid)
			return iter;
	
	return iter;
}

RCB * ResourceManager::GetResourceRcb(int rid)
{
	rcb_iter iter = RetrieveResource(rid);
	if (iter == rcbTable.end())
		return nullptr;
	else
		return *iter;
}

// 功能：进程申请资源，调用资源管理器处理
// 返回值：false表示程序需要被阻塞挂起；
int ResourceManager::Request(PCB * pcb, int rid, int reqNum)
{
	if (pcb == nullptr)
	{
		std::cout << "当前无任务在运行，请求资源非法！" << std::endl;
		return 3;
	}
	// 获取rid对应的rcb
	RCB *rcb = GetResourceRcb(rid);
	if (rcb == nullptr) {
		std::cout << "无RID为" << rid << "的资源！\n";
		return 1;	// 失败，无此资源
	}

	// 请求资源分配
	switch (rcb->RequestRes(reqNum))
	{
	case OK:	// 成功，分配资源
		pcb->AcqResource(rcb, reqNum);
		return -1;
	case BLOCK:	// 可用资源不足，挂起进程
		rcb->pushBackWaitList(pcb);
		return 0;
	case ERROR:	// 失败，申请错误
		std::cout << "资源请求量大于资源" << rcb->GetRid() << "的最大量，无法申请！" << std::endl;
		return 2;
	}

}

PCB * ResourceManager::Release(PCB * pcb, int rid, int relNum)
{
	if (pcb == nullptr)
	{
		std::cout << "当前无任务在运行，请求资源非法！" << std::endl;
		return nullptr;
	}
	// 获取rid对应的rcb
	RCB *rcb = GetResourceRcb(rid);
	if (rcb == nullptr) {
		std::cout << "无RID为" << rid << "的资源！\n";
		return nullptr;	// 失败，无此资源
	}

	switch (pcb->RelResource(rcb, relNum))
	{
	case 1:
		std::cout << "进程" << pcb->getPid()
			<< "释放资源" << rcb->GetRid() <<"量超过拥有量！\n";
		return nullptr;
	case 2:
		std::cout << "进程" << pcb->getPid()
			<< "无资源" << rcb->GetRid() << "!\n";
		return nullptr;
	default:
		break;
	}

	switch (rcb->ReleaseRes(relNum))
	{
	case true:
		return rcb->IsProcessReady();
	case false:
		std::cout << "资源归还失败！\n";
		return nullptr;
	default:
		break;
	}

	return false;
}

void ResourceManager::ShowAllResources()
{
	std::cout << "************ Resource List ************\n";
	for (rcb_iter iter = rcbTable.begin(); iter != rcbTable.end(); iter++) 
		(*iter)->ShowBriefInfo();
	std::cout << "***************************************\n";


}

void ResourceManager::ShowOneResource(int rid)
{
	GetResourceRcb(rid)->ShowAllInfo();
}

ResourceManager * ResourceManager::GetInstance()
{
	static ResourceManager instance;
	return &instance;
}

ResourceManager::ResourceManager()
{
	for (int i = 0; i < 4; i++) {
		RCB *rcb = new RCB(3);
		rcbTable.push_back(rcb);
	}

}


ResourceManager::~ResourceManager()
{
}
