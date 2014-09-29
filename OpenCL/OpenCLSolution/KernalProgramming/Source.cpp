

#define _CRT_SECURE_NO_WARNINGS
//list all opencl programs here
#define PROGRAM_FILE "hello_kernel.cl"
#define KERNEL_FUNC "hello_kernel"

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
 vectorName.sN
 where n is a number, can be used to get certain indexes, this is in hex btw, so 10 == A
 you can also use multiple number to set multiple things
 vectorName.s1CDA = 4; // sets the 2 , 12, 13, and 10th indexes 
 if a vector contains 4 elements you can use xyzw to index elements, in addition
 you can create sub vectors of any order with this notation

  v4= vector4.xxxy;
  v2= vector4.xx;
  v3= vector4.xyw;
  v1= vector4.x;
 
 hi , lo, even, and odd can be use to get certain pieces of the array...

 hi upper halve 
 lo lower halve
 even - even indices
 odd - odd indices

 you can also modify these values 

 huge tip, work-items can access local memory 100x faster than global/constant memory. Local memory is not
 as nearly as large, this is a good place to store intermediate results

 */

 /*
 the cl-denorms-are-zero (compile flag), can improve performance
 */

 /*kernal notes 
 
 every kernal declaration must start with kernal
every kernal function must return void 
some platforms will not compile kernals without arguments 

there are four possible qualifiers: __global, __constant, __local, and __private
__global these cannot be cast from __local variables in addition __global only deals with pointers 
__constant - makes data avaiable to every work item processing a kernel, it must be initialized before use
__local - if data needs to be shared among work items in a work group, but not shared with other work groups it should be declared with __local.
this is allocated once for each work-group processing the kernel and deallocated once for each work group in addition you cannot directly initialize a local item
you have to do the following __local float x; x = 4.0;
__private data must be primitives, it cannot be read back to the user, it is essentially pass by value 

it is generally good to it is a good idea to read global memory into local memory and process the data there, then when the work items are done convert it back into 
global memeory  chapters 10 -14


 */

int main() {


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
   
  
   cl_mem messageBuffer;
   size_t work_units_per_kernel;

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


   /* Create kernel for the mat_vec_mult function */
   kernel = clCreateKernel(program, KERNEL_FUNC, &err);
   if(err < 0) {
      perror("Couldn't create the kernel");
      exit(1);   
   }
   char msg[16];
   messageBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(msg), NULL, &err);

   if(err < 0) {
      perror("Couldn't create a buffer object");
      exit(1);   
   }      

   clSetKernelArg(kernel,0 , sizeof(cl_mem), &messageBuffer);

   if(err < 0) {
      perror("Couldn't set the kernel argument");
      exit(1);   
   }         


   /* Create a CL command queue for the device*/
   //CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE maybe useful
   queue = clCreateCommandQueue(context, device, 0, &err);
   if(err < 0) {
      perror("Couldn't create the command queue");
      exit(1);   
   }

   /* Enqueue the command queue to the device */
   work_units_per_kernel = 4; /* 4 work-units per kernel */ 


   
   err = clEnqueueNDRangeKernel(queue, kernel, 1, // the number of elements in a globalId

	   /*ie get_global_id(0) get_global_id(1) ... 2 dimensions */
	   
	   NULL, &work_units_per_kernel, 
      NULL, 0, NULL, NULL);
   if(err < 0) {
      perror("Couldn't enqueue the kernel execution command");
      exit(1);   
   }
   cl_uint  maxDiminsions;
   cl_uint preferredVectorWidth;
   cl_uint preFloat;
   cl_ulong size;

   // Device memory stats
   cout << "Device Memory Stats..." << endl;
   cout << "----------------------------------" << endl;
   clGetDeviceInfo(device,CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,sizeof(maxDiminsions),&maxDiminsions,NULL);
   cout <<"Max Dimensions : " << maxDiminsions << endl<<endl;
   clGetDeviceInfo(device,CL_DEVICE_GLOBAL_MEM_SIZE,sizeof(size),&size,NULL);
   cout <<"Global Memory Size : " << size << endl<<endl;
   clGetDeviceInfo(device,CL_DEVICE_GLOBAL_MEM_CACHE_SIZE,sizeof(size),&size,NULL);
   cout <<"Global Cache Memory Size : " << size << endl<<endl;
   clGetDeviceInfo(device,CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE,sizeof(size),&size,NULL);
   cout <<"Constant Buffer Size : " << size << endl<<endl;
   clGetDeviceInfo(device,CL_DEVICE_LOCAL_MEM_SIZE,sizeof(size),&size,NULL);
   cout <<"Local Memory Size: " << size << endl<<endl;


   err = clEnqueueReadBuffer(queue,messageBuffer, CL_TRUE,0 ,sizeof(msg),&msg,0,NULL,NULL);

   if(err < 0) {
      perror("Couldn't enqueue the read buffer command");
      exit(1);   
   }

   cout <<"Message : "  << msg << endl;



   clReleaseMemObject(messageBuffer);
   clReleaseKernel(kernel);
   clReleaseCommandQueue(queue);
   clReleaseProgram(program);
   clReleaseContext(context);


   system("pause");
   return 0;
}

