#include "Program.h"


Program::Program(void)
{
}


Program::~Program(void)
{
}

void Program::createProgram( Context context , Device device,char * fileName)
{
	programHandle = fopen(fileName, "r");
	if(programHandle == NULL) {
		perror("Couldn't find the program file");
		exit(1);   
	}
	fseek(programHandle, 0, SEEK_END);
	programSize = ftell(programHandle);
	rewind(programHandle);
	programBuffer = (char*)malloc(programSize + 1);
	programBuffer[programSize] = '\0';
	fread(programBuffer, sizeof(char), programSize, programHandle);
	fclose(programHandle);
	//end

	/* Create program from file */
	program = clCreateProgramWithSource(context.getId(), 1, 
		(const char**)&programBuffer, &programSize, &error);
	if(error < 0) {
		perror("Couldn't create the program");
		exit(1);   
	}
	free(programBuffer);

	const char options[] = "-cl-std=CL1.1 -cl-mad-enable -Werror";

	/* Build program */
	error = clBuildProgram(program, 0, NULL, 
		options,//compiler options go here see page 32
		NULL, NULL);
	if(error < 0) {

		/* Find size of log and print to std output */
		//different build statuses see page 34
		clGetProgramBuildInfo(program, device.getId(), CL_PROGRAM_BUILD_LOG, 
			0, NULL, &logSize);
		programLog = (char*) malloc(logSize + 1);
		programLog[logSize] = '\0';
		clGetProgramBuildInfo(program, device.getId(), CL_PROGRAM_BUILD_LOG, 
			logSize + 1, programLog, NULL);
		printf("%s\n", programLog);
		free(programLog);
		exit(1);
	}
}
