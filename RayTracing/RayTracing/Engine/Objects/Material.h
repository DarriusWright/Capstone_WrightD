#pragma once
#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif

typedef struct 
{
	cl_float3 ambient;
	cl_float3 diffuse;
	cl_float3 specular;
}Material;