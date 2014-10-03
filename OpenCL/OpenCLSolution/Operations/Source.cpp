#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include <alloca.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include "Timer.h"

//#include "radixsort_config.h"
//#include "common.h"

#ifdef  __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <vector>
#include <iostream>



struct MortonCode
{
	cl_uint code;
	cl_uint index;
};

static unsigned int GROUP_SIZE = 64;                // ATI HD7870 has 20 parallel compute units, !!!wavefront programming!!!
static unsigned int  BIN_SIZE  = 256;
static unsigned int DATA_SIZE =  (1<<17);

cl_kernel histogramKernel;
cl_kernel permuteKernel  ;
cl_kernel unifiedBlockScanKernel ;
cl_kernel blockScanKernel ;
cl_kernel prefixSumKernel;
cl_kernel blockAddKernel ;
cl_kernel mergePrefixSumsKernel   ;
cl_mem unsortedData_d    ;
cl_mem histogram_d       ;
cl_mem scannedHistogram_d;
cl_mem sortedData_d      ;
cl_mem sum_in_d          ;
cl_mem sum_out_d         ;
cl_mem summary_in_d      ;
cl_mem summary_out_d     ;
cl_command_queue commandQueue;
cl_context context ;

int
	waitAndReleaseDevice(cl_event* event) {
		cl_int status = CL_SUCCESS;
		cl_int eventStatus = CL_QUEUED;
		while(eventStatus != CL_COMPLETE) {
			clGetEventInfo(*event, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int), &eventStatus, NULL);
		}
		status = clReleaseEvent(*event);
		//CHECK_ERROR(status, CL_SUCCESS, "Failed to release event\n");
		return 0;
}

static unsigned int bitsbyte  = 8u;
static unsigned int R (1 << bitsbyte);
static unsigned int R_MASK  = 0xFFU;

// This is the threaded-historgram which builds histograms
// and bins them based on a size of 256 or 1<<8
cl_event execEvt;


void computeHistogram(int currByte) {
	cl_int status;
	size_t globalThreads = DATA_SIZE;
	size_t localThreads  = BIN_SIZE;


	status = clSetKernelArg(histogramKernel, 0, sizeof(cl_mem), (void*)&unsortedData_d);
	//std::cout << status << std::endl;
	status = clSetKernelArg(histogramKernel, 1, sizeof(cl_mem), (void*)&histogram_d);
	//std::cout << status << std::endl;

	status = clSetKernelArg(histogramKernel, 2, sizeof(cl_int), (void*)&currByte);
	//std::cout << status << std::endl;

	status = clSetKernelArg(histogramKernel, 3, sizeof(cl_int) * BIN_SIZE, NULL); 
	//std::cout << status << std::endl;


	status = clEnqueueNDRangeKernel(
		commandQueue, 
		histogramKernel,
		1,
		NULL,
		&globalThreads,
		&localThreads,
		0,
		NULL,&execEvt);
	clFlush(commandQueue);

}

void computeRankingNPermutations(int currByte, size_t groupSize) {
	cl_int status;
	//  cl_event execEvt;

	size_t globalThreads = DATA_SIZE/R;
	size_t localThreads  = groupSize;

	status = clSetKernelArg(permuteKernel, 0, sizeof(cl_mem), (void*)&unsortedData_d);
	//std::cout << status << std::endl;

	status = clSetKernelArg(permuteKernel, 1, sizeof(cl_mem), (void*)&scannedHistogram_d);
	//std::cout << status << std::endl;
	status = clSetKernelArg(permuteKernel, 2, sizeof(cl_int), (void*)&currByte);
	//std::cout << status << std::endl;
	status = clSetKernelArg(permuteKernel, 3, groupSize * R * sizeof(cl_ushort), NULL); // shared memory
	//std::cout << status << std::endl;
	status = clSetKernelArg(permuteKernel, 4, sizeof(cl_mem), (void*)&sortedData_d);
	//std::cout << status << std::endl;




	status = clEnqueueNDRangeKernel(commandQueue, permuteKernel, 1, NULL, &globalThreads, &localThreads, 0, NULL, &execEvt);
	//std::cout << status << std::endl;
	clFlush(commandQueue);
	clFinish(commandQueue);

	//waitAndReleaseDevice(&execEvt);

	cl_event copyEvt;

	status = clEnqueueCopyBuffer(commandQueue, sortedData_d, unsortedData_d, 0, 0, DATA_SIZE * sizeof(MortonCode), 0, NULL, &copyEvt);
	//std::cout << status << std::endl;
	clFlush(commandQueue);
	clFinish(commandQueue);
	//waitAndReleaseDevice(&copyEvt);
}

void computeBlockScans() {
	cl_int status;

	size_t numOfGroups = DATA_SIZE / BIN_SIZE;
	size_t globalThreads[2] = {numOfGroups, R};
	size_t localThreads[2]  = {GROUP_SIZE, 1};
	cl_uint groupSize = GROUP_SIZE;

	status = clSetKernelArg(blockScanKernel, 0, sizeof(cl_mem), (void*)&scannedHistogram_d);
	//std::cout << status << std::endl;
	status = clSetKernelArg(blockScanKernel, 1, sizeof(cl_mem), (void*)&histogram_d);
	//std::cout << status << std::endl;
	status = clSetKernelArg(blockScanKernel, 2, GROUP_SIZE * sizeof(cl_uint), NULL);
	//std::cout << status << std::endl;
	status = clSetKernelArg(blockScanKernel, 3, sizeof(cl_uint), &groupSize); 
	//std::cout << status << std::endl;
	status = clSetKernelArg(blockScanKernel, 4, sizeof(cl_mem), &sum_in_d);
	//std::cout << status << std::endl;

	// cl_event execEvt;
	status = clEnqueueNDRangeKernel(
		commandQueue,
		blockScanKernel,
		2,
		NULL,
		globalThreads,
		localThreads,
		0, 
		NULL,
		&execEvt);
	//std::cout << status << std::endl;

	clFlush(commandQueue);
	clFinish(commandQueue);



	//	std::vector<cl_uint> sums;
	//	sums.resize(DATA_SIZE);

	//	clEnqueueReadBuffer(commandQueue,sum_in_d,CL_TRUE,0, DATA_SIZE * sizeof(cl_uint) , &sums[0],0,0,0);


	//	std::vector<cl_uint> histogram;
	//histogram.resize( numOfGroups * groupSize * R);
	//

	//clEnqueueReadBuffer(commandQueue,histogram_d,CL_TRUE,0, DATA_SIZE * sizeof(cl_uint) , &histogram[0],0,0,0);
	//

	//waitAndReleaseDevice(&execEvt);

	// If there is only 1 workgroup, we will skip the block-addition and prefix-sum kernel
	if(numOfGroups/GROUP_SIZE != 1) {
		size_t globalThreadsPrefix[2] = {numOfGroups/GROUP_SIZE, R};
		status = clSetKernelArg(prefixSumKernel, 0, sizeof(cl_mem), (void*)&sum_out_d);
		//std::cout << status << std::endl;
		status = clSetKernelArg(prefixSumKernel, 1, sizeof(cl_mem), (void*)&sum_in_d);
		//std::cout << status << std::endl;
		status = clSetKernelArg(prefixSumKernel, 2, sizeof(cl_mem), (void*)&summary_in_d);
		//std::cout << status << std::endl;
		cl_uint stride = (cl_uint)numOfGroups/GROUP_SIZE;
		status = clSetKernelArg(prefixSumKernel, 3, sizeof(cl_uint), (void*)&stride);
		//std::cout << status << std::endl;
		cl_event prefixSumEvt;
		status = clEnqueueNDRangeKernel(
			commandQueue,
			prefixSumKernel,
			2,
			NULL,
			globalThreadsPrefix,
			NULL,
			0,
			NULL,
			&prefixSumEvt);
		clFlush(commandQueue);
		clFinish(commandQueue);
		//waitAndReleaseDevice(&prefixSumEvt);
		//std::cout << status << std::endl;

		// Run block-addition kernel
		cl_event execEvt2;
		size_t globalThreadsAdd[2] = {numOfGroups, R};
		size_t localThreadsAdd[2]  = {GROUP_SIZE, 1};
		status = clSetKernelArg(blockAddKernel, 0, sizeof(cl_mem), (void*)&sum_out_d);  
		//std::cout << status << std::endl;
		status = clSetKernelArg(blockAddKernel, 1, sizeof(cl_mem), (void*)&scannedHistogram_d);  
		//std::cout << status << std::endl;
		status = clSetKernelArg(blockAddKernel, 2, sizeof(cl_uint), (void*)&stride);  
		//std::cout << status << std::endl;
		status = clEnqueueNDRangeKernel(
			commandQueue,
			blockAddKernel,
			2,
			NULL,
			globalThreadsAdd,
			localThreadsAdd,
			0,
			NULL,
			&execEvt2);
		clFlush(commandQueue);
		//std::cout << status << std::endl;

		//waitAndReleaseDevice(&execEvt2);
		//clFinish(commandQueue);
		clFinish(commandQueue);


		// Run parallel array scan since we have GROUP_SIZE values which are summarized from each row
		// and we accumulate them
		size_t globalThreadsScan[1] = {R};
		size_t localThreadsScan[1] = {R};
		status = clSetKernelArg(unifiedBlockScanKernel, 0, sizeof(cl_mem), (void*)&summary_out_d);
		//std::cout << status << std::endl;
		if(numOfGroups/GROUP_SIZE != 1) 
			status = clSetKernelArg(unifiedBlockScanKernel, 1, sizeof(cl_mem), (void*)&summary_in_d); 
		else
			status = clSetKernelArg(unifiedBlockScanKernel, 1, sizeof(cl_mem), (void*)&sum_in_d); 
		//std::cout << status << std::endl;

		status = clSetKernelArg(unifiedBlockScanKernel, 2, R * sizeof(cl_uint), NULL);  // shared memory
		//std::cout << status << std::endl;
		groupSize = R;
		status = clSetKernelArg(unifiedBlockScanKernel, 3, sizeof(cl_uint), (void*)&groupSize); 
		//std::cout << status << std::endl;
		cl_event execEvt3;
		status = clEnqueueNDRangeKernel(
			commandQueue,
			unifiedBlockScanKernel,
			1,
			NULL,
			globalThreadsScan,
			localThreadsScan,
			0, 
			NULL, 
			NULL);
		//std::cout << status << std::endl;

		//clFlush(commandQueue);
		//waitAndReleaseDevice(&execEvt3);
		//clFinish(commandQueue);

		cl_event execEvt4;
		size_t globalThreadsOffset[2] = {numOfGroups, R};
		status = clSetKernelArg(mergePrefixSumsKernel, 0, sizeof(cl_mem), (void*)&summary_out_d);
		//std::cout << status << std::endl;
		status = clSetKernelArg(mergePrefixSumsKernel, 1, sizeof(cl_mem), (void*)&scannedHistogram_d);
		//std::cout << status << std::endl;
		status = clEnqueueNDRangeKernel(commandQueue, mergePrefixSumsKernel, 2, NULL, globalThreadsOffset, NULL, 0, NULL, NULL);
		//std::cout << status << std::endl;
		//clFlush(commandQueue);
		//clFinish(commandQueue);
		//waitAndReleaseDevice(&execEvt4);
	}
}

// Function that invokes the execution of the kernels
void runKernels(cl_uint* dSortedData,
				size_t numOfGroups,
				size_t groupSize) {

					int size = DATA_SIZE;
					//std::vector<cl_uint> dataVector;
					//dataVector.resize(DATA_SIZE);
					//for(int i = 0; i < size; i++)
					//{
					//	dataVector[i] = dSortedData[i];
					//}

					Timer timer;
					timer.start();
					for(int currByte = 0; currByte < sizeof(cl_uint) * bitsbyte ; currByte += bitsbyte) {
						computeHistogram(currByte);
						computeBlockScans();
						computeRankingNPermutations(currByte, groupSize);
					}

					cl_int status;
					cl_uint* data = (cl_uint*)clEnqueueMapBuffer(commandQueue, sortedData_d, CL_TRUE, CL_MAP_READ, 0, size*sizeof(cl_uint),0,NULL,NULL,&status);
					//CHECK_ERROR(status, CL_SUCCESS, "mapping buffer failed");
					memcpy(dSortedData, data, size*sizeof(cl_uint));
					timer.stop();
					float endTime = timer.interval();
					float frames = timer.getFramesPerSec();

					std::cout << "Timer : " << endTime << std::endl;
					std::cout << "Frames : " << frames << std::endl;

					std::vector<cl_uint> dataVector;
					dataVector.resize(size);
					for(int i = 0; i < size; i++)
					{
						dataVector[i] = dSortedData[i];
					}

					clEnqueueUnmapMemObject(commandQueue,sortedData_d,data,0,NULL,NULL);

					std::cout << "Done" << std::endl;
}

void runKernels(MortonCode* dSortedData,
				size_t numOfGroups,
				size_t groupSize) 
{
	int size = DATA_SIZE;

		std::vector<MortonCode> dataVector;
	dataVector.resize(size);


	Timer timer;
	timer.start();
	for(int currByte = 0; currByte < sizeof(cl_uint) * bitsbyte ; currByte += bitsbyte) {
		computeHistogram(currByte);
		computeBlockScans();
		computeRankingNPermutations(currByte, groupSize);



	}
	clEnqueueReadBuffer(commandQueue,sortedData_d,CL_TRUE,0, DATA_SIZE *  sizeof(MortonCode) , &dataVector[0],0,0,0);


	timer.stop();
	float endTime = timer.interval();
	float frames = timer.getFramesPerSec();

	std::cout << "Timer : " << endTime << std::endl;
	std::cout << "Frames : " << frames << std::endl;

	



	std::cout << "Done" << std::endl;
}


void loadProgramSource(const char** files, size_t length,char** buffer,size_t* sizes) {
	/* Read each source file (*.cl) and store the contents into a temporary datastore */
	for(size_t i=0; i < length; i++) {
		FILE* file = fopen(files[i], "r");
		if(file == NULL) {
			perror("Couldn't read the program file");
			exit(1);
		}
		fseek(file, 0, SEEK_END);
		sizes[i] = ftell(file);
		rewind(file); // reset the file pointer so that 'fread' reads from the front
		buffer[i] = (char*)malloc(sizes[i]+1);
		buffer[i][sizes[i]] = '\0';
		fread(buffer[i], sizeof(char), sizes[i], file);
		fclose(file);
	}
}

void fillRandom(cl_uint* data, unsigned int length) {
	cl_uint* iptr = data;

	for(int i = 0 ; i < length; ++i) 
		iptr[i] = (cl_uint)rand();
}

void fillRandom(MortonCode* data, unsigned int length) {
	MortonCode* iptr = data;
	for(int i = 0 ; i < length; ++i) 
	{
		iptr[i].code = (cl_uint)rand();
		iptr[i].index = i;
	}
}

int main(int argc, char** argv) {


	BIN_SIZE = (DATA_SIZE <= BIN_SIZE) ? DATA_SIZE/2 : BIN_SIZE;

	if ((DATA_SIZE / BIN_SIZE)/GROUP_SIZE  <=1)
	{
		GROUP_SIZE = (DATA_SIZE / BIN_SIZE)/2;
		if(GROUP_SIZE == 0)
		{
			GROUP_SIZE = 1;
		}

	}

	MortonCode* unsortedData = NULL;
	MortonCode* dSortedData = NULL;

	cl_platform_id* platforms;
	cl_device_id device;
	cl_uint numOfPlatforms;
	cl_int  error;
	cl_program program;
	cl_uint groupSize = GROUP_SIZE;
	cl_uint numOfGroups = DATA_SIZE / (groupSize * R); 

	unsortedData = (MortonCode*) malloc(DATA_SIZE * sizeof(MortonCode));
	fillRandom(unsortedData, DATA_SIZE);

	dSortedData = (MortonCode*) malloc(DATA_SIZE * sizeof(MortonCode));
	memset(dSortedData, 0, DATA_SIZE * sizeof(cl_uint));


	//Get the number of platforms
	//Remember that for each vendor's SDK installed on the computer,
	//the number of available platform also increased.
	error = clGetPlatformIDs(0, NULL, &numOfPlatforms);
	if(error != CL_SUCCESS) {
		perror("Unable to find any OpenCL platforms");
		exit(1);
	}

	platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * numOfPlatforms);
	printf("Number of OpenCL platforms found: %d\n", numOfPlatforms);

	error = clGetPlatformIDs(numOfPlatforms, platforms, NULL);
	if(error != CL_SUCCESS) {
		perror("Unable to find any OpenCL platforms");
		exit(1);
	}
	// Search for a GPU device through the installed platforms
	// Build a OpenCL program and do not run it.

	for(cl_int i = 0; i < numOfPlatforms; i++ ) 
	{
		// Get the GPU device
		error = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 1, &device, NULL);

		if(error != CL_SUCCESS) {
			perror("Can't locate a OpenCL compliant device i.e. GPU");
			exit(1);
		}
		// Create a context 
		context = clCreateContext(NULL, 1, &device, NULL, NULL, &error);
		if(error != CL_SUCCESS) {
			perror("Can't create a valid OpenCL context");
			exit(1);
		}

		// Load the two source files into temporary datastores
		const char *file_names[] = {"RadixSort.cl"};
		const int NUMBER_OF_FILES = 1;
		char* buffer[NUMBER_OF_FILES];
		size_t sizes[NUMBER_OF_FILES];
		loadProgramSource(file_names, NUMBER_OF_FILES, buffer, sizes);

		// Create the OpenCL program object 
		program = clCreateProgramWithSource(context, NUMBER_OF_FILES, (const char**)buffer, sizes, &error);
		if(error != CL_SUCCESS) {
			perror("Can't create the OpenCL program object");
			exit(1);
		}
		// Build OpenCL program object and dump the error message, if any 
		char *program_log;
		const char options[] = "";
		size_t log_size;

		error = clBuildProgram(program, 1, &device, options, NULL, NULL);
		if(error != CL_SUCCESS) {
			// If there's an error whilst building the program, dump the log
			clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
			program_log = (char*) malloc(log_size+1);
			program_log[log_size] = '\0';
			clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
				log_size+1, program_log, NULL);
			printf("\n=== ERROR ===\n\n%s\n=============\n", program_log);
			free(program_log);
			exit(1);
		}

		// Queue is created with profiling enabled 
		cl_command_queue_properties props;
		props |= CL_QUEUE_PROFILING_ENABLE;

		commandQueue = clCreateCommandQueue(context, device, 0, &error);
		//CHECK_ERROR(error, CL_SUCCESS, "failed to create command queue");

		unsortedData_d     = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, sizeof(MortonCode) * DATA_SIZE, unsortedData, &error);
		//CHECK_ERROR(error, CL_SUCCESS, "failed to allocate unsortedData_d");
		histogram_d        = clCreateBuffer(context, CL_MEM_READ_WRITE, numOfGroups * groupSize * R * sizeof(cl_uint), NULL, &error);
		//CHECK_ERROR(error, CL_SUCCESS, "failed to allocate histogram_d");
		scannedHistogram_d = clCreateBuffer(context, CL_MEM_READ_WRITE, numOfGroups * groupSize * R * sizeof(cl_uint), NULL, &error);
		//CHECK_ERROR(error, CL_SUCCESS, "failed to allocate scannedHistogram_d");
		sortedData_d       = clCreateBuffer(context, CL_MEM_WRITE_ONLY, DATA_SIZE * sizeof(MortonCode), NULL, &error);
		//CHECK_ERROR(error, CL_SUCCESS, "failed to allocate sortedData_d");
		sum_in_d           = clCreateBuffer(context, CL_MEM_READ_WRITE, (DATA_SIZE/groupSize) * sizeof(cl_uint), NULL, &error);
		//CHECK_ERROR(error, CL_SUCCESS, "failed to allocate sum_in_d");
		sum_out_d          = clCreateBuffer(context, CL_MEM_READ_WRITE, (DATA_SIZE/groupSize) * sizeof(cl_uint), NULL, &error);
		//CHECK_ERROR(error, CL_SUCCESS, "failed to allocate sum_out_d");
		summary_in_d       = clCreateBuffer(context, CL_MEM_READ_WRITE, R * sizeof(cl_uint), NULL, &error);
		//CHECK_ERROR(error, CL_SUCCESS, "failed to allocate summary_in_d");
		summary_out_d      = clCreateBuffer(context, CL_MEM_READ_WRITE, R * sizeof(cl_uint), NULL, &error);
		//CHECK_ERROR(error, CL_SUCCESS, "failed to allocate summary_out_d");

		histogramKernel = clCreateKernel(program, "computeHistogram", &error);
		//CHECK_ERROR(error, CL_SUCCESS, "Failed to create histogram kernel");
		permuteKernel   = clCreateKernel(program, "rankNPermute", &error);
		//CHECK_ERROR(error, CL_SUCCESS, "Failed to create permute kernel");
		unifiedBlockScanKernel  = clCreateKernel(program, "unifiedBlockScan", &error);
		//CHECK_ERROR(error, CL_SUCCESS, "Failed to create unifiedBlockScan kernel");
		blockScanKernel  = clCreateKernel(program, "blockScan", &error);
		//CHECK_ERROR(error, CL_SUCCESS, "Failed to create blockScan kernel");
		prefixSumKernel = clCreateKernel(program, "blockPrefixSum", &error);
		//CHECK_ERROR(error, CL_SUCCESS, "Failed to create compute block prefix sum kernel");
		blockAddKernel  = clCreateKernel(program, "blockAdd", &error);
		//CHECK_ERROR(error, CL_SUCCESS, "Failed to create block addition kernel");
		mergePrefixSumsKernel    = clCreateKernel(program, "mergePrefixSums", &error);
		//CHECK_ERROR(error, CL_SUCCESS, "Failed to create fix offset kernel");

		printf("elementCount: %u, numOfGroups: %u, groupSize: %u\n", DATA_SIZE, 1, groupSize);
		runKernels( dSortedData, numOfGroups, groupSize);


		// Clean up 
		for(int j=0; j< NUMBER_OF_FILES; j++) { free(buffer[j]); }

		clReleaseMemObject(unsortedData_d);
		clReleaseMemObject(histogram_d);
		clReleaseMemObject(scannedHistogram_d);
		clReleaseMemObject(sortedData_d);
		clReleaseMemObject(sum_in_d);
		clReleaseMemObject(sum_out_d);
		clReleaseMemObject(summary_out_d);
		clReleaseMemObject(summary_in_d);
		clReleaseKernel(histogramKernel);
		clReleaseKernel(permuteKernel);
		clReleaseKernel(unifiedBlockScanKernel);
		clReleaseKernel(blockScanKernel);
		clReleaseKernel(prefixSumKernel);
		clReleaseKernel(blockAddKernel);
		clReleaseKernel(mergePrefixSumsKernel);

		// Verification Checks
		/*   radixSortCPU(unsortedData, hSortedData);
		for(int k = 0, acc = 0; k < DATA_SIZE; k++) { 
		if (hSortedData[k] == dSortedData[k]) acc++;
		if ((k+1) == DATA_SIZE) {
		if (acc == DATA_SIZE) printf("Passed:%u!\n", acc); else printf("Failed:%u!\n", acc);
		}
		}*/

		free(unsortedData);
		free(dSortedData);
		// free(hSortedData);

		system("pause");

	}

}

