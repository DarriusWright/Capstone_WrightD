#pragma once

#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif
class Context
{
public:
	Context(void);
	~Context(void);
	cl_context getId();

private:
	cl_context context;
};

