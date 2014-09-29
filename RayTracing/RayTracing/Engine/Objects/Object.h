#pragma once
#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif
#include <BoundingBox\BBox.h>
#include <Objects\Material.h>
#include <glm.hpp>

class Object
{
public :
	Material material; 
	BBox box;
	glm::vec4 position;
	cl_int2 triangleIndices;
	cl_int index; 
	cl_float dummy;


};