

#define _CRT_SECURE_NO_WARNINGS
//list all opencl programs here
#define PROGRAM_FILE "id_check.cl"
#define KERNEL_FUNC "id_check"

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
 Images
 cannot be declared as arrays in the cl code
 variables cannot be pointers of these types and cannot be declared in structs 

 structs
 cannot have any pointers if the struct or pointer is used as an argument to the kernal function 
 can contain pointer is the struct or pointer is an argument to a non kernal function 

 implict vector casts are not allowed 

 */




int main(int argc, char* argv[]) {


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
   
   cl_mem vectorBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * 4, NULL, &err);

   if(err < 0) {
      perror("Couldn't create a buffer object");
      exit(1);   
   }      

   clSetKernelArg(kernel,0 , sizeof(cl_mem), &vectorBuffer);

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

   size_t dim = 2;
   size_t global_offset [] = {3,5};
   size_t global_size[] = {6,4};
   size_t local_size[] = {3,2};
   
   err = clEnqueueNDRangeKernel(queue, kernel, dim, // the number of elements in a globalId

	   /*ie get_global_id(0) get_global_id(1) ... 2 dimensions */
	   
	   global_offset, global_size, 
	   local_size, 0, NULL, NULL);
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

   int result[4];
   err = clEnqueueReadBuffer(queue,vectorBuffer, CL_TRUE,0 ,sizeof(result),&result,0,NULL,NULL);

   if(err < 0) {
      perror("Couldn't enqueue the read buffer command");
      exit(1);   
   }
   cout << "Result : ";
   for (int i = 0; i < 4; i++)
   {
	   cout << result[i] << ", ";
   }
   cout <<endl;



   clReleaseMemObject(vectorBuffer);
   clReleaseKernel(kernel);
   clReleaseCommandQueue(queue);
   clReleaseProgram(program);
   clReleaseContext(context);


   system("pause");
   return 0;
}

