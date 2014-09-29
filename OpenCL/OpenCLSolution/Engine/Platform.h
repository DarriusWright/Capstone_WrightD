#pragma once
#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <iostream>


#include <cstdlib>
#include "Device.h"

class Platform
{
public:
	Platform();
	~Platform();
	
	Device getDevice();
private:
	cl_platform_id platform;
	cl_int err;
};

