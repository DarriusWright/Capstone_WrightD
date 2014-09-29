#pragma once

#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif

#include <glm.hpp>

class BBox
{
public :
	glm::vec3 min;
	float d1;
	glm::vec3 max;
	float d2;
};