#pragma once 
#include "Material.h"
#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif

typedef struct
{
	Material material;
	//float position[4];
	cl_float4 position;
	//cl_float radius;
	inline float getMinX()
	{
		return -position.s[3]; 
	}

	inline float getMinY()
	{
		return  -position.s[3]; 
	}

	inline float getMinZ()
	{
		return -position.s[3]; 
	}

	inline float getMaxX()
	{
		return  +position.s[3]; 
	}

	inline float getMaxY()
	{
		return +position.s[3]; 
	}

	inline float getMaxZ()
	{
		return  +position.s[3]; 
	}

}Sphere;