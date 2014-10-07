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



struct MortonNode
{
	cl_uint code;
	cl_uint index;
};

static unsigned int radixGroupSize = 64;                // ATI HD7870 has 20 parallel compute units, !!!wavefront programming!!!
static unsigned int  radixBinSize  = 256;
static unsigned int radixDataSize =  512;

cl_kernel histogramKernel;
cl_kernel permuteKernel  ;
cl_kernel unifiedBlockScanKernel ;
cl_kernel blockScanKernel ;
cl_kernel prefixSumKernel;
cl_kernel blockAddKernel ;
cl_kernel mergePrefixSumsKernel;
cl_mem unsortedDataMem    ;
cl_mem histogramMem       ;
cl_mem scannedHistogramMem;
cl_mem sortedDataMem      ;
cl_mem sumInMem          ;
cl_mem sumOutMem         ;
cl_mem summaryInMem      ;
cl_mem summaryOutMem     ;
cl_command_queue queue;
cl_context context ;



static unsigned int bitsbyte  = 8u;
static unsigned int R (1 << bitsbyte);
static unsigned int R_MASK  = 0xFFU;



void computeHistogram(int currentByte) {
	cl_int status;
	size_t globalThreads = radixDataSize;
	size_t localThreads  = radixBinSize;


	status = clSetKernelArg(histogramKernel, 0, sizeof(cl_mem), (void*)&unsortedDataMem);
	status = clSetKernelArg(histogramKernel, 1, sizeof(cl_mem), (void*)&histogramMem);
	status = clSetKernelArg(histogramKernel, 2, sizeof(cl_int), (void*)&currentByte);
	status = clSetKernelArg(histogramKernel, 3, sizeof(cl_int) * radixBinSize, NULL); 
	status = clEnqueueNDRangeKernel(
		queue, 
		histogramKernel,
		1,
		NULL,
		&globalThreads,
		&localThreads,
		0,
		NULL,NULL);
}

void computeRankingNPermutations(int currentByte, size_t groupSize) {
	cl_int status;

	size_t globalThreads = radixDataSize/R;
	size_t localThreads  = groupSize;

	status = clSetKernelArg(permuteKernel, 0, sizeof(cl_mem), (void*)&unsortedDataMem);
	status = clSetKernelArg(permuteKernel, 1, sizeof(cl_mem), (void*)&scannedHistogramMem);
	status = clSetKernelArg(permuteKernel, 2, sizeof(cl_int), (void*)&currentByte);
	status = clSetKernelArg(permuteKernel, 3, groupSize * R * sizeof(cl_ushort), NULL);
	status = clSetKernelArg(permuteKernel, 4, sizeof(cl_mem), (void*)&sortedDataMem);

	status = clEnqueueNDRangeKernel(queue, permuteKernel, 1, NULL, &globalThreads, &localThreads, 0, NULL, NULL);
	status = clEnqueueCopyBuffer(queue, sortedDataMem, unsortedDataMem, 0, 0, radixDataSize * sizeof(MortonNode), 0, NULL, NULL);
}

void computeBlockScans() {
	cl_int status;

	size_t numberOfGroups = radixDataSize / radixBinSize;
	size_t globalThreads[2] = {numberOfGroups, R};
	size_t localThreads[2]  = {radixGroupSize, 1};
	cl_uint groupSize = radixGroupSize;

	status = clSetKernelArg(blockScanKernel, 0, sizeof(cl_mem), (void*)&scannedHistogramMem);
	status = clSetKernelArg(blockScanKernel, 1, sizeof(cl_mem), (void*)&histogramMem);
	status = clSetKernelArg(blockScanKernel, 2, radixGroupSize * sizeof(cl_uint), NULL);
	status = clSetKernelArg(blockScanKernel, 3, sizeof(cl_uint), &groupSize); 
	status = clSetKernelArg(blockScanKernel, 4, sizeof(cl_mem), &sumInMem);


	status = clEnqueueNDRangeKernel(
		queue,
		blockScanKernel,
		2,
		NULL,
		globalThreads,
		localThreads,
		0, 
		NULL,
		NULL);





	if(numberOfGroups/radixGroupSize != 1) {
		size_t globalThreadsPrefix[2] = {numberOfGroups/radixGroupSize, R};
		status = clSetKernelArg(prefixSumKernel, 0, sizeof(cl_mem), (void*)&sumOutMem);

		status = clSetKernelArg(prefixSumKernel, 1, sizeof(cl_mem), (void*)&sumInMem);

		status = clSetKernelArg(prefixSumKernel, 2, sizeof(cl_mem), (void*)&summaryInMem);

		cl_uint stride = (cl_uint)numberOfGroups/radixGroupSize;
		status = clSetKernelArg(prefixSumKernel, 3, sizeof(cl_uint), (void*)&stride);

		status = clEnqueueNDRangeKernel(
			queue,
			prefixSumKernel,
			2,
			NULL,
			globalThreadsPrefix,
			NULL,
			0,
			NULL,
			NULL);



		size_t globalThreadsAdd[2] = {numberOfGroups, R};
		size_t localThreadsAdd[2]  = {radixGroupSize, 1};
		status = clSetKernelArg(blockAddKernel, 0, sizeof(cl_mem), (void*)&sumOutMem);  

		status = clSetKernelArg(blockAddKernel, 1, sizeof(cl_mem), (void*)&scannedHistogramMem);  

		status = clSetKernelArg(blockAddKernel, 2, sizeof(cl_uint), (void*)&stride);  

		status = clEnqueueNDRangeKernel(
			queue,
			blockAddKernel,
			2,
			NULL,
			globalThreadsAdd,
			localThreadsAdd,
			0,
			NULL,
			NULL);


		size_t globalThreadsScan[1] = {R};
		size_t localThreadsScan[1] = {R};
		status = clSetKernelArg(unifiedBlockScanKernel, 0, sizeof(cl_mem), (void*)&summaryOutMem);

		if(numberOfGroups/radixGroupSize != 1) 
			status = clSetKernelArg(unifiedBlockScanKernel, 1, sizeof(cl_mem), (void*)&summaryInMem); 
		else
			status = clSetKernelArg(unifiedBlockScanKernel, 1, sizeof(cl_mem), (void*)&sumInMem); 


		status = clSetKernelArg(unifiedBlockScanKernel, 2, R * sizeof(cl_uint), NULL);

		groupSize = R;
		status = clSetKernelArg(unifiedBlockScanKernel, 3, sizeof(cl_uint), (void*)&groupSize); 

		status = clEnqueueNDRangeKernel(
			queue,
			unifiedBlockScanKernel,
			1,
			NULL,
			globalThreadsScan,
			localThreadsScan,
			0, 
			NULL, 
			NULL);




		size_t globalThreadsOffset[2] = {numberOfGroups, R};
		status = clSetKernelArg(mergePrefixSumsKernel, 0, sizeof(cl_mem), (void*)&summaryOutMem);

		status = clSetKernelArg(mergePrefixSumsKernel, 1, sizeof(cl_mem), (void*)&scannedHistogramMem);

		status = clEnqueueNDRangeKernel(queue, mergePrefixSumsKernel, 2, NULL, globalThreadsOffset, NULL, 0, NULL, NULL);

	}
}

void radixSort(MortonNode* dSortedData,
			   size_t numberOfGroups,
			   size_t groupSize) 
{
	int size = radixDataSize;

	std::vector<MortonNode> dataVector;
	dataVector.resize(size);


	Timer timer;
	timer.start();
	for(int currentByte = 0; currentByte < sizeof(cl_uint) * bitsbyte ; currentByte += bitsbyte) {
		computeHistogram(currentByte);
		computeBlockScans();
		computeRankingNPermutations(currentByte, groupSize);


	}



	timer.stop();
	float endTime = timer.interval();
	float frames = timer.getFramesPerSec();

	std::cout << "Timer : " << endTime << std::endl;
	std::cout << "Frames : " << frames << std::endl;
	std::cout << "Done" << std::endl;

	clEnqueueReadBuffer(queue,sortedDataMem,CL_TRUE,0, radixDataSize *  sizeof(MortonNode) , &dataVector[0],0,0,0);



}


void radixSort(cl_uint* dSortedData,
			   size_t numberOfGroups,
			   size_t groupSize) {

				   int size = radixDataSize;
				   Timer timer;
				   timer.start();
				   for(int currentByte = 0; currentByte < sizeof(cl_uint) * bitsbyte ; currentByte += bitsbyte) {
					   computeHistogram(currentByte);
					   computeBlockScans();
					   computeRankingNPermutations(currentByte, groupSize);
				   }

				   cl_int status;
				   cl_uint* data = (cl_uint*)clEnqueueMapBuffer(queue, sortedDataMem, CL_TRUE, CL_MAP_READ, 0, size*sizeof(cl_uint),0,NULL,NULL,&status);

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

				   clEnqueueUnmapMemObject(queue,sortedDataMem,data,0,NULL,NULL);

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

void fillRandom(MortonNode* data, unsigned int length) {
	MortonNode* iptr = data;
	for(int i = 0 ; i < length; i += 16) 
	{
		cl_uint value = (cl_uint)rand() + 10000;
		for(int j = 0; j < 16; j++)
		{
			iptr[i + j].code = value;
			iptr[i + j].index = i+ j;
		}
	}
}

int main(int argc, char** argv) {


	radixBinSize = (radixDataSize <= radixBinSize) ? radixDataSize/2 : radixBinSize;

	if ((radixDataSize / radixBinSize)/radixGroupSize  <=1)
	{
		radixGroupSize = (radixDataSize / radixBinSize)/2;
		if(radixGroupSize == 0)
		{
			radixGroupSize = 1;
		}

	}

	std::vector<MortonNode> unsortedData;// = NULL;
	MortonNode* dSortedData = NULL;

	cl_platform_id* platforms;
	cl_device_id device;
	cl_uint numOfPlatforms;
	cl_int  error;
	cl_program program;
	cl_uint groupSize = radixGroupSize;
	cl_uint numberOfGroups = radixDataSize / (groupSize * R); 

	//unsortedData = (MortonNode*) malloc(radixDataSize * sizeof(MortonNode));
	unsortedData.resize(radixDataSize);
	fillRandom(&unsortedData[0], radixDataSize);

	dSortedData = (MortonNode*) malloc(radixDataSize * sizeof(MortonNode));
	memset(dSortedData, 0, radixDataSize * sizeof(cl_uint));


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

		queue = clCreateCommandQueue(context, device, 0, &error);

		unsortedDataMem     = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, sizeof(MortonNode) * radixDataSize, &unsortedData[0], &error);
		histogramMem        = clCreateBuffer(context, CL_MEM_READ_WRITE, numberOfGroups * groupSize * R * sizeof(cl_uint), NULL, &error);
		scannedHistogramMem = clCreateBuffer(context, CL_MEM_READ_WRITE, numberOfGroups * groupSize * R * sizeof(cl_uint), NULL, &error);
		sortedDataMem       = clCreateBuffer(context, CL_MEM_WRITE_ONLY, radixDataSize * sizeof(MortonNode), NULL, &error);
		sumInMem           = clCreateBuffer(context, CL_MEM_READ_WRITE, (radixDataSize/groupSize) * sizeof(cl_uint), NULL, &error);
		sumOutMem          = clCreateBuffer(context, CL_MEM_READ_WRITE, (radixDataSize/groupSize) * sizeof(cl_uint), NULL, &error);
		summaryInMem       = clCreateBuffer(context, CL_MEM_READ_WRITE, R * sizeof(cl_uint), NULL, &error);
		summaryOutMem      = clCreateBuffer(context, CL_MEM_READ_WRITE, R * sizeof(cl_uint), NULL, &error);

		histogramKernel = clCreateKernel(program, "computeHistogram", &error);
		permuteKernel   = clCreateKernel(program, "rankNPermute", &error);
		unifiedBlockScanKernel  = clCreateKernel(program, "unifiedBlockScan", &error);
		blockScanKernel  = clCreateKernel(program, "blockScan", &error);
		prefixSumKernel = clCreateKernel(program, "blockPrefixSum", &error);
		blockAddKernel  = clCreateKernel(program, "blockAdd", &error);
		mergePrefixSumsKernel    = clCreateKernel(program, "mergePrefixSums", &error);

		radixSort( dSortedData, numberOfGroups, groupSize);

		// Clean up 

		clReleaseMemObject(unsortedDataMem);
		clReleaseMemObject(histogramMem);
		clReleaseMemObject(scannedHistogramMem);
		clReleaseMemObject(sortedDataMem);
		clReleaseMemObject(sumInMem);
		clReleaseMemObject(sumOutMem);
		clReleaseMemObject(summaryOutMem);
		clReleaseMemObject(summaryInMem);
		clReleaseKernel(histogramKernel);
		clReleaseKernel(permuteKernel);
		clReleaseKernel(unifiedBlockScanKernel);
		clReleaseKernel(blockScanKernel);
		clReleaseKernel(prefixSumKernel);
		clReleaseKernel(blockAddKernel);
		clReleaseKernel(mergePrefixSumsKernel);

		//free(unsortedData);
		free(dSortedData);

		system("pause");

	}

}

