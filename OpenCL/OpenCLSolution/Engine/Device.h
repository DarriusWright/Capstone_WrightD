#pragma once
#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif

#include "Context.h"

class Device
{
public:
	Device(void);
	~Device(void);
	Context createContext();
	cl_device_id getId();
private:
	cl_device_id device;
};

