#include "ServiceContainer.h"


ServiceContainer::ServiceContainer() : mServices()
{
}


ServiceContainer::~ServiceContainer()
{
}

void  ServiceContainer::addService(UINT typeId, void * service)
{
	mServices.insert(std::pair<UINT, void*>(typeId, service));
}
void  ServiceContainer::removeService(UINT typeId)
{
	mServices.erase(typeId);
}
void* ServiceContainer::getService(UINT typeId)const
{
	std::map<UINT, void *>::const_iterator it = mServices.find(typeId);
	return (it != mServices.end()) ? it->second : nullptr;
}
