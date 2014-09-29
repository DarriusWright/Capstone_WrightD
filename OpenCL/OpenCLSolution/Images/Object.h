#pragma once
#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif
#include "BBox.h"
#include "Material.h"

class Object
{
public :
	Material material; 
	BBox box;
	cl_float4 position;
	cl_int2 triangleIndices;
	cl_int index; 
	cl_float dummy;


};