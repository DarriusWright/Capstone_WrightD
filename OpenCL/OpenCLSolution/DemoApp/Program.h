#pragma once
#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif


#include <cstdlib>
#include <iostream>

class Program
{
public:
	Program(char * fileName );
	~Program(void);
	void initialize();
private:
	cl_program program;
	FILE * program_handle;
	char * fileName;
	char * programBuffer;

};

