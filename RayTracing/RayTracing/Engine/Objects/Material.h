#pragma once
#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif


enum MaterialType
{
	DIFFUSE,
	SPECULAR,
	MIRROR
};

typedef struct 
{
	cl_float4 ambient;
	cl_float4 diffuse;
	cl_float4 specular;
	float reflection;
	float refraction;
	MaterialType type;
	float d[1];
}Material;