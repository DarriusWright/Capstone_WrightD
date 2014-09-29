#pragma once
#include <Misc\Common.h>
class ServiceContainer
{
public:
	ServiceContainer();
	void addService(UINT typeId, void * service);
	void removeService(UINT typeId);
	void *getService(UINT typeId)const;
	~ServiceContainer();

private:
	ServiceContainer(const ServiceContainer & rhs);
	ServiceContainer&operator=(const ServiceContainer& rhs);

	std::map<UINT, void *>mServices;
};

