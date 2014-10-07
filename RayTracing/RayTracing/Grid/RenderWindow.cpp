#include "RenderWindow.h"

/*
CL_DEVICE_MAX_WORK_ITEM_SIZES - 1024, 1024, 64
*/
const cl_float RenderWindow::MIN = 10000000.0f;
const cl_float RenderWindow::MAX = -10000000.0f;

RenderWindow::RenderWindow(void) : multi(2.0f),camera(glm::vec3(0.0f,0,50.0f), glm::vec3(0,0,0)) , random(Random::getInstance())
{
	BBox b = {glm::vec3(MIN,MIN,MIN),0.0f,glm::vec3(MAX,MAX,MAX),0.0f};
	box = b;
	setSamples(4);
	construct();
}
RenderWindow::~RenderWindow(void)
{
	delete[]readBuffer;
	clReleaseMemObject(clImage);

	clReleaseMemObject(writeCLImage);
	clReleaseSampler(sampler);
	clReleaseKernel(drawSceneKernel);
	clReleaseKernel(sceneBBoxKernel);
	clReleaseKernel(initializeCellsKernel);
	clReleaseKernel(findObjectCellsKernel);
	clReleaseCommandQueue(queue);
	clReleaseProgram(program);
	clReleaseContext(context);

	//	releaseUpdate();

}
void RenderWindow::setSamples(int samples)
{
	this->samples = samples;
	this->sampleSquared = sqrt(samples);
}
void RenderWindow::addMesh(std::string fileName)
{
	Assimp::Importer importer;
	const aiScene * scene = importer.ReadFile(fileName, flags);
	aiMesh * mesh = (scene->mMeshes[0]);

	std::vector<cl_uint> modelIndices;
	std::vector<glm::vec3> modelVertices;
	modelVertices.reserve(mesh->mNumVertices);

	if (mesh->HasFaces())
	{
		int faceCount = mesh->mNumFaces;
		for (UINT i = 0; i < faceCount; i++)
		{
			aiFace * face = &mesh->mFaces[i];
			for (UINT j = 0; j < face->mNumIndices; j++)
			{
				modelIndices.push_back(face->mIndices[j]);
			}
		}
	}


	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		modelVertices.push_back(glm::vec3(*reinterpret_cast<glm::vec3*>(&mesh->mVertices[i])));
	}

	if(triangles.size())
		clReleaseMemObject(trianglesMem);

	//int triangleInitSize = triangles.size();
	triangles.reserve(modelIndices.size() / 3);
	objects.reserve(modelIndices.size() / 3);
	Random random = Random::getInstance();

	Material material = {{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)}, 
	{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)},
	{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)}};

	glm::vec3 position(0,0,10);
	for (UINT i = 0; i < modelIndices.size(); i+=3)
	{
		Triangle tri;
		tri.v0 = position + glm::vec3(*reinterpret_cast<glm::vec3*>(&mesh->mVertices[modelIndices[i]]));
		tri.v1 = position + glm::vec3(*reinterpret_cast<glm::vec3*>(&mesh->mVertices[modelIndices[i + 1]]));
		tri.v2 = position + glm::vec3(*reinterpret_cast<glm::vec3*>(&mesh->mVertices[modelIndices[i + 2]]));

		Object o = createObjectFromTriangle(tri);
		o.triangleIndex = triangles.size();
		o.index = objects.size();
		o.material = material;
		objects.push_back(o);
		triangles.push_back(tri);

	}

	//trianglesMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Triangle)* triangles.size(), &triangles[0],&err);

}
void RenderWindow::resizeEvent(QResizeEvent * e)
{
	//windowWidth = width();
	//windowHeight = height();
	//delete [] readBuffer;
	//readBuffer = new uchar [width() * height() * 4];
}
void RenderWindow::addLight(Light light)
{
	lights.push_back(light);
	numberOfLights++;
}
void RenderWindow::addObject(Object object)
{
	objects.push_back(object);
	numberOfObjects++;
}
void RenderWindow::addSphere()
{
	Sphere sphere = {{{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)}, 
	{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)},
	{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)}},
	glm::vec4(random.getRandomFloat(-100.0f,100.0f), random.getRandomFloat(-100.0f,100.0f), random.getRandomFloat(30.0f,50.0f),30.0f)};
	//spheres.push_back(sphere);

	Object object; 

	BBox b = {
		glm::vec3(sphere.getMinX(),
		sphere.getMinY(),
		sphere.getMinZ()),0.0f,
		glm::vec3(
		sphere.getMaxX(),
		sphere.getMaxY(),
		sphere.getMaxZ()),0.0f

	};
	object.material = sphere.material;
	object.box = b;
	//cl_int2 indices = {0,0};
	object.triangleIndex =-1;
	object.index = objects.size();
	object.position = sphere.position;


	objects.push_back(object);
}
void RenderWindow::construct()
{
	setMinimumSize(640,480);

	layout = new QHBoxLayout();
	Light light = {{{0.925,0.835,0.102}, {0.73,0.724,0.934},{0.2,0.52,0.96}}, {2.0f,2.0f,200.0f}};
	addMesh("D:/Capstone/RayTracing/RayTracing/Grid/basicCube.obj");

	//Random random = Random::getInstance();
	//objects.reserve(NUMBER_OF_SPHERES);
	//for(int i = 0; i < NUMBER_OF_SPHERES; i++)
	//{

	//	Sphere sphere = {{{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)}, 
	//	{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)},
	//	{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)}},
	//	glm::vec4(random.getRandomFloat(-100.0f,100.0f), random.getRandomFloat(-100.0f,100.0f), random.getRandomFloat(30.0f,50.0f),30.0f)};
	//	//spheres.push_back(sphere);

	//	Object object; 

	//	BBox b = {
	//		glm::vec3(sphere.getMinX(),
	//		sphere.getMinY(),
	//		sphere.getMinZ()),0.0f,
	//		glm::vec3(
	//		sphere.getMaxX(),
	//		sphere.getMaxY(),
	//		sphere.getMaxZ()),0.0f

	//	};
	//	object.material = sphere.material;
	//	object.box = b;
	//	//cl_int2 indices = {0,0};
	//	object.triangleIndex =-1;
	//	object.index = i;
	//	object.position = sphere.position;


	//	objects.push_back(object);
	//

	//	//box.min[0] = glm::min((sphere.getMinX() + sphere.position[0]) ,box.min[0]);
	//	//box.min[1] = glm::min(sphere.getMinY()  +  sphere.position[1],box.min[1]);
	//	//box.min[2] = glm::min(sphere.getMinZ()	 + sphere.position[2],box.min[2]);

	//	//box.max[0] = glm::max(sphere.getMaxX()+ sphere.position[0],box.max[0]);
	//	//box.max[1] = glm::max(sphere.getMaxY()+  sphere.position[1],box.max[1]);
	//	//box.max[2] = glm::max(sphere.getMaxZ()+ sphere.position[2],box.max[2]);


	//}




	lights.push_back(light);
	numberOfObjects = objects.size();
	numberOfLights = lights.size();

	layout->addWidget(&imageLabel);
	setLayout(layout);

	windowWidth = width();
	windowHeight = height();
	readBuffer = new uchar [windowWidth * windowHeight * 4];

	initializeCL();

	//connect(&updateTimer,SIGNAL(timeout()),this,SLOT(updateScene()));
	//updateTimer.start();

	//for (int i = 0; i < 5; i++)
	//{
	//	camera.moveLeft();

	//}

	QImage r(readBuffer,windowWidth,windowHeight,QImage::Format::Format_RGBA8888);
	readImage  =r;
}
void RenderWindow::updateDrawScene()
{
	// invalid mem object on the second run... check it out.
	//err |= clSetKernelArg(drawSceneKernel,0 , sizeof(cl_mem), &clImage);
	//err |= clSetKernelArg(drawSceneKernel,1 , sizeof(cl_mem), &writeCLImage);
	//err |= clSetKernelArg(drawSceneKernel,2 , sizeof(cl_sampler), &sampler);
	//err |= clSetKernelArg(drawSceneKernel,3 , sizeof(cl_int), &windowWidth);
	//err |= clSetKernelArg(drawSceneKernel,4 , sizeof(cl_int), &windowHeight);
	//err |= clSetKernelArg(drawSceneKernel,5 , sizeof(cl_mem),(objects.size()) ?  &objectMem : 0);
	//err |= clSetKernelArg(drawSceneKernel,6 , sizeof(cl_mem),(triangles.size()) ?  &trianglesMem : 0);
	//err |= clSetKernelArg(drawSceneKernel,7 , sizeof(cl_mem),(lights.size()) ?  &lightMem : 0);
	//err |= clSetKernelArg(drawSceneKernel,8 , sizeof(cl_int), &numberOfObjects);
	//err |= clSetKernelArg(drawSceneKernel,9 , sizeof(cl_int), &numberOfLights);
	//err |= clSetKernelArg(drawSceneKernel,10, sizeof(BBox), &box);
	//err |= clSetKernelArg(drawSceneKernel, 11, sizeof(cl_mem), &cellsMem);
	//err |= clSetKernelArg(drawSceneKernel,12 , sizeof(cl_int), &nx);
	//err |= clSetKernelArg(drawSceneKernel,13 , sizeof(cl_int), &ny);
	//err |= clSetKernelArg(drawSceneKernel,14 , sizeof(cl_int), &nz);
	//err |= clSetKernelArg(drawSceneKernel, 15, sizeof(cl_mem), &cellIndicesMem);
	//err |= clSetKernelArg(drawSceneKernel, 16, sizeof(cl_mem), &objectIndicesMem);
	//err |= clSetKernelArg(drawSceneKernel, 17, sizeof(Camera), &camera);

	err |= clSetKernelArg(drawSceneKernel,0 , sizeof(cl_mem), &clImage);
	err |= clSetKernelArg(drawSceneKernel,1 , sizeof(cl_mem), &writeCLImage);
	err |= clSetKernelArg(drawSceneKernel,2 , sizeof(cl_sampler), &sampler);
	err |= clSetKernelArg(drawSceneKernel,3 , sizeof(cl_int), &windowWidth);
	err |= clSetKernelArg(drawSceneKernel,4 , sizeof(cl_int), &windowHeight);
	err |= clSetKernelArg(drawSceneKernel,5 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(drawSceneKernel,6 , sizeof(cl_mem), &trianglesMem);

	err |= clSetKernelArg(drawSceneKernel,7 , sizeof(cl_mem), &lightMem);
	err |= clSetKernelArg(drawSceneKernel,8 , sizeof(cl_int), &numberOfObjects);
	err |= clSetKernelArg(drawSceneKernel,9 , sizeof(cl_int), &numberOfLights);
	err |= clSetKernelArg(drawSceneKernel,10, sizeof(BBox), &box);
	err |= clSetKernelArg(drawSceneKernel, 11, sizeof(cl_mem), &cellsMem);
	err |= clSetKernelArg(drawSceneKernel,12 , sizeof(cl_int), &nx);
	err |= clSetKernelArg(drawSceneKernel,13 , sizeof(cl_int), &ny);
	err |= clSetKernelArg(drawSceneKernel,14 , sizeof(cl_int), &nz);
	err |= clSetKernelArg(drawSceneKernel, 15, sizeof(cl_mem), &cellIndicesMem);
	err |= clSetKernelArg(drawSceneKernel, 16, sizeof(cl_mem), &objectIndicesMem);
	err |= clSetKernelArg(drawSceneKernel, 17, sizeof(Camera), &camera);
	err |= clSetKernelArg(drawSceneKernel,18 , sizeof(cl_int), &samples);
	err |= clSetKernelArg(drawSceneKernel,19 , sizeof(cl_int), &sampleSquared);






	err |= clEnqueueNDRangeKernel(queue, drawSceneKernel, 2,
		NULL, &globalWorkSize[0], 
		NULL, 0, NULL, NULL);

	err |= clEnqueueReadImage(queue,writeCLImage,CL_TRUE,origin,region,0,0,readBuffer,0,NULL,NULL);
}
void RenderWindow::updateScene()
{
	releaseUpdate();
	timer.start();	
	handleKeyInput();
	camera.update();

	// 0
	profileTimer.start();

	updateBBox();
	updateCells();
	updateDrawScene();

	imageLabel.setPixmap(QPixmap::fromImage(readImage));
	float drawTimer = profileTimer.elapsed()/1000.0f;

	interval = (timer.elapsed()/1000.0f);
	fps = 1.0f/interval;

}
void RenderWindow::updateBBox()
{
	BBox b = {glm::vec3(MIN,MIN,MIN),0.0f,glm::vec3(MAX,MAX,MAX),0.0f};

	box = b;
	objectMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Object)* objects.size(), &objects[0],&err);
	if(triangles.size())
		trianglesMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Triangle)* triangles.size(), &triangles[0],&err);
	lightMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Light) * lights.size(), &lights[0],&err);
	boundingBoxMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,sizeof(BBox), &box,&err);

	//err |= clEnqueueReadBuffer(queue,trianglesMem, CL_TRUE,0, sizeof(Triangle) * triangles.size(), &triangles[0], 0,0,0);

	cl_int lengthOfObjects = objects.size();

	err |= clSetKernelArg(sceneBBoxKernel,0 , sizeof(cl_mem), &boundingBoxMem);
	err |= clSetKernelArg(sceneBBoxKernel,1 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(sceneBBoxKernel,2 , sizeof(cl_int), &lengthOfObjects);
	err |= clSetKernelArg(sceneBBoxKernel,3 , sizeof(cl_mem),&minMem);
	err |= clSetKernelArg(sceneBBoxKernel,4 , sizeof(cl_mem) , &maxMem);

	if(err != CL_SUCCESS) {
		perror("Couldn't set the sceneBBoxKernel argument");
		exit(1);   
	} 

	sceneBBoxGlobalWorkSize = objects.size();
	err |= clEnqueueNDRangeKernel(queue, sceneBBoxKernel, 1 ,
		NULL, &sceneBBoxGlobalWorkSize, 
		NULL, 0, NULL, NULL);

	err |=clEnqueueReadBuffer(queue,boundingBoxMem,CL_TRUE,0, sizeof(BBox), &box,0,0,0);

}
void RenderWindow::updateCells()
{
	initCellWorkSize[0] = objects.size();

	wx =box.max[0] - box.min[0];
	wy =box.max[1] - box.min[1];
	wz =box.max[2] - box.min[2];

	s = pow((wx * wy * wz)/objects.size(), 0.3333333);

	nx = multi * wx / s + 1;
	ny = multi * wy / s + 1;
	nz = multi * wz / s + 1;

	numCells = nx * ny * nz;
	numCells = numCells;


	cells.resize(numCells);
	cBoxes.resize(numCells);
	initCellWorkSize[1] = numCells;
	numberOfCellObjects = 0;
	cellsMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(int) * numCells, &cells[0],&err);
	cellsBoxMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(BBox) * numCells, &cBoxes[0],&err);
	sumMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(int), &numberOfCellObjects,&err);


	err|=clSetKernelArg(initializeCellsKernel,0 , sizeof(cl_mem), &objectMem);
	err|=clSetKernelArg(initializeCellsKernel,1 , sizeof(cl_mem), &boundingBoxMem);
	err|=clSetKernelArg(initializeCellsKernel,2 , sizeof(cl_mem) , &cellsMem);
	err|=clSetKernelArg(initializeCellsKernel,3 , sizeof(cl_int), &nx);
	err|=clSetKernelArg(initializeCellsKernel,4 , sizeof(cl_int), &ny);
	err|=clSetKernelArg(initializeCellsKernel,5 , sizeof(cl_int), &nz);
	err|=clSetKernelArg(initializeCellsKernel, 6, sizeof(cl_mem),&cellsBoxMem );
	err|=clSetKernelArg(initializeCellsKernel, 7, sizeof(cl_mem), &sumMem );

	if(err != CL_SUCCESS) {
		perror("Couldn't set the initializeCellsKernel argument");
		exit(1);   
	}


	err|=clEnqueueNDRangeKernel(queue, initializeCellsKernel, 2 ,
		NULL, initCellWorkSize, 
		NULL, 0, NULL, NULL);

	err|=clEnqueueReadBuffer(queue,cellsMem,CL_TRUE,0, sizeof(int) * numCells, &cells[0],0,0,0);
	err|=clEnqueueReadBuffer(queue,cellsBoxMem,CL_TRUE,0, sizeof(BBox) * numCells, &cBoxes[0],0,0,0);
	err|=clEnqueueReadBuffer(queue,sumMem,CL_TRUE,0, sizeof(cl_int), &numberOfCellObjects,0,0,0);


	cellIndices.resize(numCells);


	cellIndices[0] = cells[0]; 
	for (int i = 1; i < numCells; i++)
	{
		cellIndices[i] = cells[i] + cellIndices[i-1];
	}

	cellIncrements.resize(numCells);
	objectIndices.resize(numberOfCellObjects);


	cellIndicesMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, cellIndices.size() * sizeof(cl_int),&cellIndices[0], &err);
	objectIndicesMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR, sizeof(cl_int) * numberOfCellObjects, &objectIndices[0], &err );
	cellIncrementsMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR, sizeof(cl_int)* numCells, &cellIncrements[0], &err);


	err|=clSetKernelArg(findObjectCellsKernel,0 , sizeof(cl_mem), &objectMem);
	err|=clSetKernelArg(findObjectCellsKernel,1 , sizeof(cl_mem), &boundingBoxMem);
	err|=clSetKernelArg(findObjectCellsKernel,2 , sizeof(cl_mem) , &cellsMem);
	err|=clSetKernelArg(findObjectCellsKernel,3 , sizeof(cl_mem) , &cellIndicesMem);
	err|=clSetKernelArg(findObjectCellsKernel,4 , sizeof(cl_mem)  , &objectIndicesMem);
	err|=clSetKernelArg(findObjectCellsKernel,5 , sizeof(cl_mem) , &cellsBoxMem);
	err|=clSetKernelArg(findObjectCellsKernel,6 , sizeof(cl_mem) , &cellIncrementsMem);
	err|=clEnqueueNDRangeKernel(queue, findObjectCellsKernel, 2 ,
		NULL, initCellWorkSize, 
		NULL, 0, NULL, NULL);

}
float RenderWindow::getFPS()
{
	return fps;
}
float RenderWindow::getInterval()
{
	return interval;
}
void RenderWindow::releaseUpdate()
{
	cellIndices.clear();
	objectIndices.clear();
	cellIncrements.clear();
	cBoxes.clear();
	cells.clear();
	clReleaseMemObject(sumMem);
	clReleaseMemObject(objectMem);//
	clReleaseMemObject(lightMem);//
	clReleaseMemObject(cellsMem);//
	clReleaseMemObject(cellsBoxMem);//
	clReleaseMemObject(objectIndicesMem);//
	clReleaseMemObject(cellIndicesMem);//
	clReleaseMemObject(cellIncrementsMem);//
	clReleaseMemObject(boundingBoxMem);
	clReleaseMemObject(trianglesMem);//

	//clReleaseMemObject(minMem);//
	//clReleaseMemObject(maxMem);//
	//if(triangles.size())
	//{
	//	clReleaseMemObject(trianglesMem);//
	//}

}
cl_program RenderWindow::buildProgram(std::string files [], int numberOfFiles)
{
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

	char ** program_buffer = new char*[numberOfFiles]; 
	size_t * program_size = new size_t[numberOfFiles];
	for(int i = 0; i < numberOfFiles; i++)
	{
		const char * fileName =files[i].c_str(); 
		program_handle = fopen(fileName, "r");
		if(program_handle == NULL) {
			perror("Couldn't find the program file");
			exit(1);   
		}
		fseek(program_handle, 0, SEEK_END);
		program_size[i] = ftell(program_handle);
		rewind(program_handle);

		// read kernel source into buffer
		program_buffer[i] = (char*) malloc(program_size[i]+ 1);
		program_buffer[i][program_size[i]] = '\0';
		fread(program_buffer[i], sizeof(char), program_size[i] +1 , program_handle);
		fclose(program_handle);
		program_buffer[i][program_size[i]] = '\0';
		int start = program_size[i]-1;
		while(program_buffer[i][start] == 'Í')
		{
			program_buffer[i][start--] = ' ';
		}

	}
	//end

	/* Create program from file */
	cl_program program = clCreateProgramWithSource(context, numberOfFiles, 
		(const char**)program_buffer, program_size, &err);
	if(err < 0) {
		perror("Couldn't create the program");
		exit(1);   
	}

	for (int i = 0; i < numberOfFiles; i++)
	{
		free(program_buffer[i]);
	}
	delete [] program_buffer;
	delete [] program_size;
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

	return program;
}
void RenderWindow::handleKeyInput()
{
	if(GetAsyncKeyState('D'))
	{
		camera.moveRight();
	}
	if(GetAsyncKeyState('S'))
	{
		camera.moveDown();
	}
	if(GetAsyncKeyState('W'))
	{
		camera.moveUp();
	}
	if(GetAsyncKeyState('A'))
	{
		camera.moveLeft();
	}
	if(GetAsyncKeyState('R'))
	{
		camera.moveIn();
	}
	if(GetAsyncKeyState('F'))
	{
		camera.moveOut();
	}

}
void RenderWindow::initializeProgram()
{
	std::string files []  = { "Kernels/kernelHelperFunctions.opencl", "Kernels/generateSceneBBox.opencl","Kernels/initializeCells.opencl","Kernels/findObjectCells.opencl", "Kernels/drawScene.opencl"};
	program = buildProgram(files, 5);
	drawSceneKernel = clCreateKernel(program, "drawScene", &err);
	sceneBBoxKernel = clCreateKernel(program, "generateSceneBBox", &err);
	initializeCellsKernel = clCreateKernel(program, "initializeCells", &err);
	findObjectCellsKernel = clCreateKernel(program,"findObjectCells", &err);
}
void RenderWindow::initializeSceneBBox()
{
	cl_int lengthOfObjects = objects.size();
	int globalObjectSize = nextPowerOfTwo(objects.size());

	vector<cl_float3> minArr;
	minArr.resize(globalObjectSize);
	vector<cl_float3> maxArr;
	maxArr.resize(globalObjectSize);

	minMem = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(cl_float3) * globalObjectSize, &minArr[0],&err);
	maxMem = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(cl_float3) * globalObjectSize, &maxArr[0],&err);
	//finish setting up mem stuff in header file...

	err |= clSetKernelArg(sceneBBoxKernel,0 , sizeof(cl_mem), &boundingBoxMem);
	err |= clSetKernelArg(sceneBBoxKernel,1 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(sceneBBoxKernel,2 , sizeof(cl_int), &lengthOfObjects);
	err |= clSetKernelArg(sceneBBoxKernel,3 , sizeof(cl_mem),&minMem);
	err |= clSetKernelArg(sceneBBoxKernel,4 , sizeof(cl_mem) , &maxMem);

	if(err != CL_SUCCESS) {
		perror("Couldn't set the sceneBBoxKernel argument");
		exit(1);   
	} 


	queue = clCreateCommandQueue(context, device, 0, &err);
	if(err < 0) {
		perror("Couldn't create the command queue");
		exit(1);   
	}


	sceneBBoxGlobalWorkSize = nextPowerOfTwo(objects.size());
	err = clEnqueueNDRangeKernel(queue, sceneBBoxKernel, 1 ,
		NULL, &sceneBBoxGlobalWorkSize, 
		NULL, 0, NULL, NULL);

	clEnqueueReadBuffer(queue,boundingBoxMem,CL_TRUE,0, sizeof(BBox), &box,0,0,0);


}
const cl_uint RenderWindow::NUMBER_OF_SPHERES = 2;
void RenderWindow::initializeCells()
{
	initCellWorkSize[0] = objects.size();

	float wx =box.max[0] - box.min[0];
	float wy =box.max[1] - box.min[1];
	float wz =box.max[2] - box.min[2];

	float s = pow((wx * wy * wz)/objects.size(), 0.3333333);

	nx = multi * wx / s + 1;
	ny = multi * wy / s + 1;
	nz = multi * wz / s + 1;

	numCells = nx * ny * nz;
	numCells = numCells;
	numberOfCellObjects = 0;
	//vector<cl_uint> cells;
	cells.resize(numCells);
	//vector<BBox> cBoxes;
	cBoxes.resize(numCells);
	initCellWorkSize[1] = numCells;
	cellsMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(int) * numCells, &cells[0],&err);
	cellsBoxMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(BBox) * numCells, &cBoxes[0],&err);
	sumMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(int), &numberOfCellObjects,&err);


	err |= clSetKernelArg(initializeCellsKernel,0 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(initializeCellsKernel,1 , sizeof(cl_mem), &boundingBoxMem);
	err |= clSetKernelArg(initializeCellsKernel,2 , sizeof(cl_mem) , &cellsMem);
	err |= clSetKernelArg(initializeCellsKernel,3 , sizeof(cl_int), &nx);
	err |= clSetKernelArg(initializeCellsKernel,4 , sizeof(cl_int), &ny);
	err |= clSetKernelArg(initializeCellsKernel,5 , sizeof(cl_int), &nz);
	err |= clSetKernelArg(initializeCellsKernel, 6, sizeof(cl_mem),&cellsBoxMem );
	err |= clSetKernelArg(initializeCellsKernel, 7, sizeof(cl_mem), &sumMem );

	if(err != CL_SUCCESS) {
		perror("Couldn't set the initializeCellsKernel argument");
		exit(1);   
	}

	err = clEnqueueNDRangeKernel(queue, initializeCellsKernel, 2 ,
		NULL, initCellWorkSize, 
		NULL, 0, NULL, NULL);
	clEnqueueReadBuffer(queue,cellsMem,CL_TRUE,0, sizeof(int) * numCells, &cells[0],0,0,0);
	clEnqueueReadBuffer(queue,cellsBoxMem,CL_TRUE,0, sizeof(BBox) * numCells, &cBoxes[0],0,0,0);
	clEnqueueReadBuffer(queue,sumMem,CL_TRUE,0, sizeof(cl_int), &numberOfCellObjects,0,0,0);


	cellIndices.resize(numCells);

	cellIndices[0] = cells[0]; 
	for (int i = 1; i < numCells; i++)
	{
		cellIndices[i] = cells[i] + cellIndices[i-1];
	}

	cellIncrements.resize(numCells);
	objectIndices.resize(numberOfCellObjects);


	cellIndicesMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, cellIndices.size() * sizeof(cl_int),&cellIndices[0], &err);
	objectIndicesMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR, sizeof(cl_int) * numberOfCellObjects, &objectIndices[0], &err );
	cellIncrementsMem = clCreateBuffer(context, CL_MEM_COPY_HOST_PTR, sizeof(cl_int)* numCells, &cellIncrements[0], &err);

}
void RenderWindow::initializeCellObjects()
{
	err |= clSetKernelArg(findObjectCellsKernel,0 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(findObjectCellsKernel,1 , sizeof(cl_mem), &boundingBoxMem);
	err |= clSetKernelArg(findObjectCellsKernel,2 , sizeof(cl_mem) , &cellsMem);
	err |= clSetKernelArg(findObjectCellsKernel,3 , sizeof(cl_mem) , &cellIndicesMem);
	err |= clSetKernelArg(findObjectCellsKernel,4 , sizeof(cl_mem)  , &objectIndicesMem);
	err |= clSetKernelArg(findObjectCellsKernel,5 , sizeof(cl_mem) , &cellsBoxMem);
	err |= clSetKernelArg(findObjectCellsKernel,6 , sizeof(cl_mem) , &cellIncrementsMem);

	if(err != CL_SUCCESS) {
		perror("Couldn't set the findObjectCells argument");
		exit(1);   
	} 

	err = clEnqueueNDRangeKernel(queue, findObjectCellsKernel, 2 ,
		NULL, initCellWorkSize, 
		NULL, 0, NULL, NULL);

	//	clEnqueueReadBuffer(queue,objectIndicesMem,CL_TRUE,0, sizeof(int) * numberOfCellObjects, &objectIndices[0],0,0,0);
	//	clEnqueueReadBuffer(queue,cellIncrementsMem,CL_TRUE,0,sizeof(int) * numCells, &cellIncrements[0],0,0,0);

}
void RenderWindow::initializeDrawScene()
{

	err |= clSetKernelArg(drawSceneKernel,0 , sizeof(cl_mem), &clImage);
	err |= clSetKernelArg(drawSceneKernel,1 , sizeof(cl_mem), &writeCLImage);
	err |= clSetKernelArg(drawSceneKernel,2 , sizeof(cl_sampler), &sampler);
	err |= clSetKernelArg(drawSceneKernel,3 , sizeof(cl_int), &windowWidth);
	err |= clSetKernelArg(drawSceneKernel,4 , sizeof(cl_int), &windowHeight);
	err |= clSetKernelArg(drawSceneKernel,5 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(drawSceneKernel,6 , sizeof(cl_mem), &trianglesMem);

	err |= clSetKernelArg(drawSceneKernel,7 , sizeof(cl_mem), &lightMem);
	err |= clSetKernelArg(drawSceneKernel,8 , sizeof(cl_int), &numberOfObjects);
	err |= clSetKernelArg(drawSceneKernel,9 , sizeof(cl_int), &numberOfLights);
	err |= clSetKernelArg(drawSceneKernel,10, sizeof(BBox), &box);
	err |= clSetKernelArg(drawSceneKernel, 11, sizeof(cl_mem), &cellsMem);
	err |= clSetKernelArg(drawSceneKernel,12 , sizeof(cl_int), &nx);
	err |= clSetKernelArg(drawSceneKernel,13 , sizeof(cl_int), &ny);
	err |= clSetKernelArg(drawSceneKernel,14 , sizeof(cl_int), &nz);
	err |= clSetKernelArg(drawSceneKernel, 15, sizeof(cl_mem), &cellIndicesMem);
	err |= clSetKernelArg(drawSceneKernel, 16, sizeof(cl_mem), &objectIndicesMem);
	err |= clSetKernelArg(drawSceneKernel, 17, sizeof(Camera), &camera);
	err |= clSetKernelArg(drawSceneKernel,18 , sizeof(cl_int), &samples);
	err |= clSetKernelArg(drawSceneKernel,19 , sizeof(cl_int), &sampleSquared);


	if(err != CL_SUCCESS) {
		perror("Couldn't set the drawSceneKernel argument");
		exit(1);   
	}         
	globalWorkSize[0] = windowWidth;
	globalWorkSize[1] =  windowHeight;


	err = clEnqueueNDRangeKernel(queue, drawSceneKernel, 2 ,
		NULL, globalWorkSize, 
		NULL, 0, NULL, NULL);



	if(err < 0) {
		perror("Couldn't enqueue the drawSceneKernel execution command");
		exit(1);   
	}

	origin[0] = 0;
	origin[1] = 0;
	origin[2] = 0;
	region[0] = windowWidth;
	region[1] = windowHeight;
	region[2] = 1;
	err = clEnqueueReadImage(queue,writeCLImage,CL_TRUE,origin,region,0,0,readBuffer,0,NULL,NULL);

	if(err != CL_SUCCESS)
	{
		cout << "Error creating reading from buffer" << endl;
		exit(-1);
	}
}
void RenderWindow::initializeMemory()
{
	cl_image_format clImageFormat;
	clImageFormat.image_channel_order = CL_RGBA;
	clImageFormat.image_channel_data_type = CL_UNSIGNED_INT8;


	objectMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Object)* objects.size(), &objects[0],&err);
	if(err != CL_SUCCESS)
	{
		cout << "Error creating Object" << endl;
		exit(-1);
	}

	lightMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Light)* lights.size(), &lights[0],&err);
	if(err != CL_SUCCESS)
	{
		cout << "Error creating Lights Object" << endl;
		exit(-1);
	}

	if(triangles.size() > 0)
	{

		trianglesMem = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(Triangle) * triangles.size(), &triangles[0], &err);

		if(err != CL_SUCCESS)
		{
			cout << "Error creating triangle Object" << endl;
			exit(-1);
		}
	}
	clImage = clCreateImage2D(context, CL_MEM_READ_ONLY , & clImageFormat, windowWidth,windowHeight,0,NULL,&err);

	if(err != CL_SUCCESS)
	{
		cout << "Error creating CL Image object" << endl;
		exit(-1);
	}

	writeCLImage = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &clImageFormat, windowWidth,windowHeight, 0 ,NULL, &err);

	if(err != CL_SUCCESS)
	{
		cout << "Error creating writable CL Image object" << endl;
		exit(-1);
	}

	sampler  = clCreateSampler(context, CL_FALSE, CL_ADDRESS_CLAMP_TO_EDGE, CL_FILTER_NEAREST, & err);
	if(err != CL_SUCCESS)
	{
		cout << "Error creating CL sampler object" << endl;
		//cleanup
		exit(-1);
	}

	boundingBoxMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,sizeof(BBox), &box,&err);

	if(err != CL_SUCCESS)
	{
		cout << "Error creating bounding box memory object" << endl;
		//cleanup
		exit(-1);
	}




	readBuffer = new uchar [windowWidth*windowHeight * 4];

}
void RenderWindow::initializeCL()
{

	initializeProgram();
	initializeMemory();
	profileTimer.start();
	timer.start();
	float time = timer.elapsed()/ 1000.0f;
	initializeSceneBBox();
	cout << "SceneBox : " << (profileTimer.elapsed()/1000.0f) << endl;
	profileTimer.start();
	initializeCells();
	cout << "Initialize Cells : " << (profileTimer.elapsed()/1000.0f) << endl;
	profileTimer.start();
	initializeCellObjects();
	cout << "Initialize Cell Objects : " << (profileTimer.elapsed()/1000.0f) << endl;
	profileTimer.start();
	initializeDrawScene();
	cout << "DrawScene : " << (profileTimer.elapsed()/1000.0f) << endl;
	cout << "Done!" << endl;
}
