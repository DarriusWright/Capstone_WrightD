#pragma once
#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "Context.h"
#include "Device.h"
#include <cstdlib>

class Program
{
public:
	Program(void);
	~Program(void);


private:
	void createProgram(Context context, Device device, char * fileName);
	cl_program program;
	FILE * programHandle;
	char * programBuffer;
	char * programLog;
	size_t programSize;
	size_t logSize;
	int error;
};

