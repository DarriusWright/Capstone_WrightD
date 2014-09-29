

#define _CRT_SECURE_NO_WARNINGS
//list all opencl programs here
#define PROGRAM_FILE "matvec.cl"
#define KERNEL_FUNC "matvec_mult"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif
#include <cstdlib>

#include <iostream>
 using std::cout;
 using std::endl;

 /*
 Max work dimensions  :3 // when working with images this should be 2 or 3
 */


 /* ---------------------OpenCL------------------------------------------
 platform obtains the vendors implementations of opencl
 platform is used to get the devices 
 the opencl host chooses which devices to use 
 opencl can give the same kernal to multiple devices through command queues 
 
 
 */


 /* Creating Images
 there are two functions for creating images 
 clCreateImage2D
 (
	cl_context context,  // the context currently being used 
	cl_mem_flags opts, // the same as other memory allocations, it declared read only, write only, etc
	const cl_image_format * format, 
	//the image format is a data structure that is made up off channel order and channel type
	//the order being CL_RGB,CL_RGBA,CL_ARGB, CL_A ,CLRB, and so on (see og 49)
	//the channel type or data type can be some of the following
	//CL_HALF_FLOAT, CL_FLOAT, CL_UNSIGNED_INT, CL_SIGNED_INT in there various forms
	//CL_UNSIGNED_INT8 is probably your best bet with CL_RGBA


	size_t width, // image width
	size_t height, // image height
	size_t row_pitch,
	void  * data, cl_int * error
 )
 and 
 clCreateImage3D(
 	cl_context context,  // the context currently being used 
	cl_mem_flags opts,
	const cl_image_format * format, 
	size_t width, // image width
	size_t height, // image height
	size_t depth, // 
	size_t row_pitch,
	void  * data, cl_int * error
 )

 */


 /*Memory object transfer commands 
 A host creates a command queue when it wants to access a device. The host communicates with the device by dispatching commands
 to the queue, which is also known as enqueuing.
 There are various functions that all start with clEnqueue that enqueue data transfer commands 
 the three main are read/write, map and unmap memory, and functions that copy data between memory objects 
 
 To read a buffer object from a device you can use one of the simpliest functions clEnqueueReadBuffer, one of the 
 six functions for reading and writing data.

 the image functions, which are the ones we are most interested in are 
 clEnqueueReadImage
 (
	cl_command_queue queue,
	cl_mem image,
	cl_bool blocking,
	const size_t origin[3], // the starting place on the image 
	const size_t region[3], // how much the image to transfer out (width, height, depth)
	size_t row_pitch, 
	size_t slice_pitch, 
	void * ptr,
	cl_uint num_events,
	cosnt cl_event * wait_list,
	cl_event * event
 )
 clEnqueueWriteImage
 (
	cl_command_queue queue,
	cl_mem image,
	cl_bool blocking,
	const size_t origin[3], 
	const size_t region[3],
	size_t row_pitch, 
	size_t slice_pitch, 
	void * ptr,
	cl_uint num_events,
	cosnt cl_event * wait_list,
	cl_event * event
	);


 */


 /* Data partitioning 
 if you are implementing an algoritm with opencl, you probably have a great deal of data 
 to process. This makes data partitioning a priority, the better you distribute the faster computational
 tasks will be finished.

 Most OpenCl devices contain serverl processing elements,and with the right code tou can control how much
 data each processing element receives.

 There is only one function to know clEnqueueNDRangeKernal, one of the most and important yet complex functions that 
 OpenCL has to offer.

 clEnqueueNDRangeKernal(
 cl_command_queue queue,
 cl_kernel kernel,
 cl_uint work_dims,  // the number of dimensions in the data ... generallly 2 or 3 for images 
 const size_t * global_work_offset, // the global id offsets in each dimension
 const size_t * global_work_size, // the number of work items in each dimension
 // how many work items need to be processed i.e. {4,6,9 } 
 // f 0-4 {f 2 - 8 {f 3 - 12} } ... offset {0,2,3} 

 const size_t * local_work_size, // the number of work-items in a work group, in each dimension
 cl_uint num_events,
 const cl_event * wait_list,
 cl_event * event
 )

 
 */


 void printInfo( )
 {
	 cl_platform_info name;
	cl_platform_id * platforms;
	cl_device_id * devices;
    cl_uint num;
    cl_uint ok = 1;
	clGetPlatformIDs(0, NULL, &num);
	platforms = (cl_platform_id*)malloc(num * sizeof(cl_platform_id));
	
	clGetPlatformIDs(num, platforms, 0);
	char queryBuffer[1024];
	for(int i = 0; i < num ; i++)
	{
	clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME,1024,&queryBuffer,NULL);
	cout << "NAME : " << queryBuffer << endl << endl;
	clGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE,1024,&queryBuffer,NULL);
	cout << "PROFILE : "<< queryBuffer << endl << endl;
	clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR,1024,&queryBuffer,NULL);
	cout << "VENDOR : " << queryBuffer << endl << endl;
	clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION,1024,&queryBuffer,NULL);
	cout << "VERSION : " << queryBuffer << endl << endl;
	clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS,1024,&queryBuffer,NULL);
	cout << "EXTENSIONS : " << queryBuffer << endl << endl;


	cl_uint num_devices;
	clGetDeviceIDs(platforms[i],CL_DEVICE_TYPE_ALL,0,NULL,&num_devices);

	cout << "Number Of Devices : " << num_devices << endl;

	devices = (cl_device_id*)malloc(num_devices * sizeof(cl_device_id));
	clGetDeviceIDs(platforms[i],CL_DEVICE_TYPE_ALL,num_devices,devices,0);
	cout <<"Device Info" << endl<<endl;
	int queryInt;
	for (int i = 0; i < num_devices; i++)
	{
		clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(queryBuffer), &queryBuffer, NULL);
		cout << "Name : " << queryBuffer << endl;
		clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(queryBuffer), &queryBuffer, NULL);
		cout << "Vendor : " << queryBuffer << endl;
		clGetDeviceInfo(devices[i], CL_DEVICE_VERSION, sizeof(queryBuffer), &queryBuffer, NULL);
		cout << "Version : "<< queryBuffer << endl;

		clGetDeviceInfo(devices[i], CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(int), &queryInt, NULL);
		cout << "Time Resolution : " << queryInt << endl;

		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(int), &queryInt, NULL);
		cout << "Compute Units : " << queryInt << endl;
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(int), &queryInt, NULL);
		cout << "Clock Freqency : " << queryInt << endl;



	}
	delete [] devices;

	}

	
	
	delete [] platforms;

	cout <<endl << endl;
 }

 cl_uint getNumberOfKernals(cl_program program)
 {
	 cl_uint numKernals;

	 clCreateKernelsInProgram(program,0,0,&numKernals);
	 return numKernals;
 }

 void printKernalInfo(cl_kernel kernal)
 {
	 char kernalName[20];
	 /*CL_KERNAL_FUNCTION_NAME
	 this is important when you need to search through an array of kernals to find one that corresponds to a function 
	 */
	 clGetKernelInfo(kernal,CL_KERNEL_FUNCTION_NAME,sizeof(kernalName),kernalName, NULL);
	 
	 cout<< kernalName<< endl;
	 cout <<"---------------------" << endl;
 }

 


int main() {

	printInfo();
	/*
	creating multiple programs

	the program buffer needs to be a pointer array with the number of files as the size
	there also needs to be a fileName pointer array with the names of the programs as its contents
	in addition make program size an array that is the number of files 


	*/


   /* Host/device data structures */
   cl_platform_id platform;
   cl_device_id device;
   cl_context context;
   cl_command_queue queue;
   cl_int i, err;
   
   /* Program/kernel data structures */
   cl_program program;
   FILE *program_handle;
   char *program_buffer, *program_log;
   size_t program_size, log_size;
   cl_kernel kernel;
   
   /* Data and buffers */
   float mat[16], vec[4], result[4];
   float correct[4] = {0.0f, 0.0f, 0.0f, 0.0f};
   cl_mem mat_buff, vec_buff, res_buff;
   size_t work_units_per_kernel;

   /* Initialize data to be processed by the kernel */
   for(i=0; i<16; i++) {
      mat[i] = i * 2.0f;
   } 
   for(i=0; i<4; i++) {
      vec[i] = i * 3.0f;
      correct[0] += mat[i]    * vec[i];
      correct[1] += mat[i+4]  * vec[i];
      correct[2] += mat[i+8]  * vec[i];
      correct[3] += mat[i+12] * vec[i];      
   }

   /* Identify a platform */
   err = clGetPlatformIDs(1, &platform, NULL);
   if(err < 0) {
      perror("Couldn't find any platforms");
      exit(1);
   }

   /* Access a device */
   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, 
         &device, NULL);
   if(err < 0) {
      perror("Couldn't find any devices");
      exit(1);
   }

   /* Create the context */
   context = clCreateContext(NULL, 1, &device, NULL, 
         NULL, &err);
   if(err < 0) {
      perror("Couldn't create a context");
      exit(1);   
   }

   /* Read program file and place content into buffer
   when doing multiple programs for each through them all here
   */
   program_handle = fopen(PROGRAM_FILE, "r");
   if(program_handle == NULL) {
      perror("Couldn't find the program file");
      exit(1);   
   }
   fseek(program_handle, 0, SEEK_END);
   program_size = ftell(program_handle);
   rewind(program_handle);
   program_buffer = (char*)malloc(program_size + 1);
   program_buffer[program_size] = '\0';
   fread(program_buffer, sizeof(char), program_size, program_handle);
   fclose(program_handle);
   //end

   /* Create program from file */
   program = clCreateProgramWithSource(context, 1, 
      (const char**)&program_buffer, &program_size, &err);
   if(err < 0) {
      perror("Couldn't create the program");
      exit(1);   
   }
   free(program_buffer);

   const char options[] = "-cl-std=CL1.1 -cl-mad-enable -Werror";

   /* Build program */
   err = clBuildProgram(program, 0, NULL, 
	   options,//compiler options go here see page 32
	   NULL, NULL);
   if(err < 0) {

      /* Find size of log and print to std output */
	   //different build statuses see page 34
      clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 
            0, NULL, &log_size);
      program_log = (char*) malloc(log_size + 1);
      program_log[log_size] = '\0';
      clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 
            log_size + 1, program_log, NULL);
      printf("%s\n", program_log);
      free(program_log);
      exit(1);
   }

   /* Creating kernals 
   there is a simple function for creating kernals clCreateKernalsInProgram, this will create a kernal for each 
   function in the program
   if you just want a single kernal the clCreateKernal is the function
   */

   /* Create kernel for the mat_vec_mult function */
   kernel = clCreateKernel(program, KERNEL_FUNC, &err);
   if(err < 0) {
      perror("Couldn't create the kernel");
      exit(1);   
   }

   /*Kernal Info*/
   cout << "Number Of Kernals" << getNumberOfKernals(program) << endl;
   printKernalInfo(kernel);

   /* Create CL buffers to hold input and output data */
   
   /*take note that this is read only memory that is being passed in ,
   if you want to create a buffer object to hold the kernals 
   output data then its a good idea to make the buffer object only
   so the device will only write to it 

   CL_MEM_USE_HOST_PTR - if you are running host applications on a memory limited system
   CL_MEM_COPY_HOST_PTR - if memory is not an issues and you prefer reliable data transfer

   */
   mat_buff = clCreateBuffer(context, CL_MEM_READ_ONLY | 
      CL_MEM_COPY_HOST_PTR, sizeof(float)*16, mat, &err);


   if(err < 0) {
      perror("Couldn't create a buffer object");
      exit(1);   
   }      
   vec_buff = clCreateBuffer(context, CL_MEM_READ_ONLY | 
      CL_MEM_COPY_HOST_PTR, sizeof(float)*4, vec, NULL);

   // there is only one arguement in this second call because data
   // isn'y allocated on the host
   res_buff = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 
      sizeof(float)*4, NULL, NULL);

   /* Create kernel arguments from the CL buffers */

   /*
   the last arguement of set kernal arg points to the data that will be transfered 
   to the kernal function, this can be pointer to primitive data, to a memory object, or to a sampler object
   (transfers to an object that defines how images are read)
   
   */

	/*
	creating a subbuffer

	cl_buffer_region region;// the object used to create the sub-buffer
   region.size = 40 * sizeof(float); // how much of the memory that you would like 
   region.origin = 50*sizeof(float); // how far into the memorry to start
   // function call for creating a sub-buffer
   cl_mem subBuffer = clCreateSubBuffer(mainBuffer, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, CL_BUFFER_CREATE_TYPE_REGION, & region,&err)


	*/


   cout << "Size of cl_mem : " <<  sizeof(cl_mem) << endl;

   err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &mat_buff);
   if(err < 0) {
      perror("Couldn't set the kernel argument");
      exit(1);   
   }         
   clSetKernelArg(kernel, 1, sizeof(cl_mem), &vec_buff);
   clSetKernelArg(kernel, 2, sizeof(cl_mem), &res_buff);

   /* Create a CL command queue for the device*/
   //CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE maybe useful
   queue = clCreateCommandQueue(context, device, 0, &err);
   if(err < 0) {
      perror("Couldn't create the command queue");
      exit(1);   
   }

   /* Enqueue the command queue to the device */
   work_units_per_kernel = 4; /* 4 work-units per kernel */ 

   //simple enqueueing
  // err = clEnqueueTask(queue,kernel,0,NULL,NULL);
   
   
   err = clEnqueueNDRangeKernel(queue, kernel, 1, // the number of elements in a globalId

	   /*ie get_global_id(0) get_global_id(1) ... 2 dimensions */
	   
	   NULL, &work_units_per_kernel, 
      NULL, 0, NULL, NULL);
   if(err < 0) {
      perror("Couldn't enqueue the kernel execution command");
      exit(1);   
   }
   cl_uint  maxDiminsions;
   clGetDeviceInfo(device,CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,sizeof(maxDiminsions),&maxDiminsions,NULL);
   cout <<"Max Dimensions : " << maxDiminsions << endl<<endl;

   /* Read the result */
   err = clEnqueueReadBuffer(queue,// the queue being used 
	   res_buff, // the memory buffer that will yield the results 
	   CL_TRUE, // blocking -the function will not return until the read / write operation is finish
	   //if false then the function will not wait for the data transfer
	   0, // offset
	   sizeof(float)*4,  // size
      result, // result 
	  0,  // the rest pertains to events 
	  NULL, 
	  NULL);
   if(err < 0) {
      perror("Couldn't enqueue the read buffer command");
      exit(1);   
   }

   /* Test the result */
   if((result[0] == correct[0]) && (result[1] == correct[1])
      && (result[2] == correct[2]) && (result[3] == correct[3])) {
      printf("Matrix-vector multiplication successful.\n");
   }
   else {
      printf("Matrix-vector multiplication unsuccessful.\n");
   }

   cout << "x : " << result[0] <<  "\ty : " << result[1] <<  "\tz : " << result[2] << endl;


   /* Deallocate resources */
   clReleaseMemObject(mat_buff);
   clReleaseMemObject(vec_buff);
   clReleaseMemObject(res_buff);
   clReleaseKernel(kernel);
   clReleaseCommandQueue(queue);
   clReleaseProgram(program);
   clReleaseContext(context);


   system("pause");
   return 0;
}

