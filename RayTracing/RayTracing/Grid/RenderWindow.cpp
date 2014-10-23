#include "RenderWindow.h"

/*
CL_DEVICE_MAX_WORK_ITEM_SIZES - 1024, 1024, 64
*/
const cl_float RenderWindow::MIN = 10000000.0f;
const cl_float RenderWindow::MAX = -10000000.0f;
const cl_uint RenderWindow::NUMBER_OF_SPHERES = 1;


RenderWindow::RenderWindow(void) : multi(2.0f),camera(glm::vec3(0.0f,0,20.0f), glm::vec3(0,0,0)) , random(Random::getInstance())
{
	BBox b = {glm::vec3(MIN,MIN,MIN),0.0f,glm::vec3(MAX,MAX,MAX),0.0f};
	box = b;
	setSamples(1);

	initializeProgram();
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
	releaseUpdate();

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
		for (int i = 0; i < faceCount; i++)
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

	if(objects.size())
		clReleaseMemObject(objectMem);


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
		tri.v0 = glm::vec3(*reinterpret_cast<glm::vec3*>(&mesh->mVertices[modelIndices[i]]));
		tri.v1 = glm::vec3(*reinterpret_cast<glm::vec3*>(&mesh->mVertices[modelIndices[i + 1]]));
		tri.v2 = glm::vec3(*reinterpret_cast<glm::vec3*>(&mesh->mVertices[modelIndices[i + 2]]));

		Object o = createObjectFromTriangle(tri, position);
		tri.v0 = position + glm::vec3(*reinterpret_cast<glm::vec3*>(&mesh->mVertices[modelIndices[i]]));
		tri.v1 = position + glm::vec3(*reinterpret_cast<glm::vec3*>(&mesh->mVertices[modelIndices[i + 1]]));
		tri.v2 = position + glm::vec3(*reinterpret_cast<glm::vec3*>(&mesh->mVertices[modelIndices[i + 2]]));

		if(mesh->HasNormals())
		{
			tri.normal = glm::vec3(*reinterpret_cast<glm::vec3*>(&mesh->mNormals[modelIndices[i]]));
		}
		
		o.triangleIndex = triangles.size();
		o.index = objects.size();
		o.material = material;
		objects.push_back(o);
		triangles.push_back(tri);
		//box.max = glm::max(box.max, tri.v0);
		//box.max = glm::max(box.max, tri.v1);
		//box.max = glm::max(box.max, tri.v2);


		//box.min = glm::min(box.min, tri.v0);
		//box.min = glm::min(box.min, tri.v1);
		//box.min = glm::min(box.min, tri.v2);
	}


	trianglesMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Triangle)* triangles.size(), &triangles[0],&err);
	objectMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Object)* objects.size(), &objects[0],&err);




	//box.min -= 0.1f;
	//box.max += 0.1f;

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
void RenderWindow::addObject(std::string path)
{
	objectPaths.push_back(path);
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
	Light light = {{{0.925f,0.835f,0.102f}, {0.73f,0.724f,0.934f},{0.2f,0.52f,0.96f}}, {2.0f,2.0f,200.0f}};
	addMesh("suzy2.obj");

	//Random random = Random::getInstance();
	//for(int i = 0; i < NUMBER_OF_SPHERES; i++)
	//{

	//	Sphere sphere = {{{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)}, 
	//	{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)},
	//	{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)}},
	//	glm::vec4(random.getRandomFloat(-10.0f,10.0f), random.getRandomFloat(-10.0f,10.0f), random.getRandomFloat(0.0f,10.0f),2.0f)};
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
	//	cl_int2 indices = {0,0};
	//	object.triangleIndex =-1;
	//	object.index = i;
	//	object.position = sphere.position;


	//	objects.push_back(object);
	//	float x;
	//	float y;

	//	box.min[0] = glm::min((sphere.getMinX() + sphere.position[0]) ,box.min[0]);
	//	box.min[1] = glm::min(sphere.getMinY()  +  sphere.position[1],box.min[1]);
	//	box.min[2] = glm::min(sphere.getMinZ()	 + sphere.position[2],box.min[2]);

	//	box.max[0] = glm::max(sphere.getMaxX()+ sphere.position[0],box.max[0]);
	//	box.max[1] = glm::max(sphere.getMaxY()+  sphere.position[1],box.max[1]);
	//	box.max[2] = glm::max(sphere.getMaxZ()+ sphere.position[2],box.max[2]);


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


	QImage r(readBuffer,windowWidth,windowHeight,QImage::Format::Format_RGBA8888);
	readImage = r;
}
void RenderWindow::updateDrawScene()
{
	setUpDrawSceneArgs();
	err |= clEnqueueNDRangeKernel(queue, drawSceneKernel, 2,
		NULL, &globalWorkSize[0], 
		NULL, 0, NULL, NULL);

	err |= clEnqueueReadImage(queue,writeCLImage,CL_TRUE,origin,region,0,0,readBuffer,0,NULL,NULL);
	//qDebug() << err;
}
void RenderWindow::updateScene()
{
	//releaseUpdate();
	timer.start();	
	handleKeyInput();
	camera.update();

	// 0
	profileTimer.start();

	//updateBBox();
	//updateCells();
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

	numberOfObjects = objects.size();
	int globalObjectSize = nextPowerOfTwo(objects.size());

	minArr.resize(globalObjectSize);
	maxArr.resize(globalObjectSize);

	minMem = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(cl_float3) * globalObjectSize, &minArr[0],&err);
	maxMem = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(cl_float3) * globalObjectSize, &maxArr[0],&err);


	objectMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Object)* objects.size(), &objects[0],&err);
	if(triangles.size())
		trianglesMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Triangle)* triangles.size(), &triangles[0],&err);
	lightMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Light) * lights.size(), &lights[0],&err);
	boundingBoxMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,sizeof(BBox), &box,&err);

	//err |= clEnqueueReadBuffer(queue,trianglesMem, CL_TRUE,0, sizeof(Triangle) * triangles.size(), &triangles[0], 0,0,0);
	setUpSceneBoxArgs();


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
	initCellWorkSize = objects.size();
	calculateVoxelSize();



	cells.resize(totalVoxels);
	numberOfCellObjects = 0;
	cellsMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(int) * totalVoxels, &cells[0],&err);
	sumMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(int), &numberOfCellObjects,&err);
	//1391 = 1
	setUpCellArgs();

	if(err != CL_SUCCESS) {
		perror("Couldn't set the initializeCellsKernel argument");
		exit(1);   
	}


	err|=clEnqueueNDRangeKernel(queue, initializeCellsKernel, 1 ,
		NULL, &initCellWorkSize, 
		NULL, 0, NULL, NULL);

	err|=clEnqueueReadBuffer(queue,cellsMem,CL_TRUE,0, sizeof(int) * totalVoxels, &cells[0],0,0,0);
	err|=clEnqueueReadBuffer(queue,sumMem,CL_TRUE,0, sizeof(cl_int), &numberOfCellObjects,0,0,0);


	cellIndices.resize(totalVoxels);


	cellIndices[0] = cells[0]; 
	for (int i = 1; i < totalVoxels; i++)
	{
		cellIndices[i] = cells[i] + cellIndices[i-1];
	}

	cellIncrements.resize(totalVoxels);
	objectIndices.resize(numberOfCellObjects);


	cellIndicesMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, cellIndices.size() * sizeof(cl_int),&cellIndices[0], &err);
	objectIndicesMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR, sizeof(cl_int) * numberOfCellObjects, &objectIndices[0], &err );
	cellIncrementsMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR, sizeof(cl_int)* totalVoxels, &cellIncrements[0], &err);

	setUpCellObjectArgs();
	err|=clEnqueueNDRangeKernel(queue, findObjectCellsKernel, 1 ,
		NULL, &initCellWorkSize, 
		NULL, 0, NULL, NULL);


	clEnqueueReadBuffer(queue,objectIndicesMem,CL_TRUE,0, sizeof(int) * numberOfCellObjects, &objectIndices[0],0,0,0);
}



float RenderWindow::getFPS()
{
	return fps;
}
float RenderWindow::getInterval()
{
	return interval;
}

void RenderWindow::releaseDrawScene()
{
	if(triangles.size())clReleaseMemObject(trianglesMem);
	if(objects.size())clReleaseMemObject(objectMem);//
	if(lights.size())clReleaseMemObject(lightMem);//
}
void RenderWindow::releaseCells()
{
	clReleaseMemObject(sumMem);
	clReleaseMemObject(cellsMem);//
	clReleaseMemObject(objectIndicesMem);//
	clReleaseMemObject(cellIndicesMem);//
	clReleaseMemObject(cellIncrementsMem);//

}
void RenderWindow::releaseBBox()
{
	clReleaseMemObject(boundingBoxMem);
	clReleaseMemObject(minMem);
	clReleaseMemObject(maxMem);
}
void RenderWindow::releaseUpdate()
{

	if(objectPaths.size() > 0)
	{
		for (uint i = 0; i < objectPaths.size(); i++)
		{
			addMesh(objectPaths[i]);
		}

		objectPaths.clear();
	}
	cellIndices.clear();
	objectIndices.clear();
	cellIncrements.clear();
	minArr.clear();
	maxArr.clear();
	cells.clear();

	releaseBBox();
	releaseCells();
	releaseDrawScene();


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
		while(program_buffer[i][start] == '�')
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
	const char options[] = "-cl-std=CL1.1 -cl-mad-enable -Werror -cl-nv-verbose";

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
	numberOfObjects = objects.size();
	int globalObjectSize = nextPowerOfTwo(objects.size());

	minArr.resize(globalObjectSize);
	maxArr.resize(globalObjectSize);

	minMem = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(cl_float3) * globalObjectSize, &minArr[0],&err);
	maxMem = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(cl_float3) * globalObjectSize, &maxArr[0],&err);
	//finish setting up mem stuff in header file...

	setUpSceneBoxArgs();

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

	err |= clEnqueueReadBuffer(queue,boundingBoxMem,CL_TRUE,0, sizeof(BBox), &box,0,0,0);
}
void RenderWindow::initializeCells()
{
	QElapsedTimer t;
	t.start();
	initCellWorkSize = objects.size();

	calculateVoxelSize();

	numberOfCellObjects = 0;
	cells.resize(totalVoxels);
	cellsMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(int) * totalVoxels, &cells[0],&err);
	sumMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(int), &numberOfCellObjects,&err);


	setUpCellArgs();

	if(err != CL_SUCCESS) {
		perror("Couldn't set the initializeCellsKernel argument");
		exit(1);   
	}

	err = clEnqueueNDRangeKernel(queue, initializeCellsKernel, 1 ,
		NULL, &initCellWorkSize, 
		NULL, 0, NULL, NULL);

	err |= clEnqueueReadBuffer(queue,cellsMem,CL_TRUE,0, sizeof(int) * totalVoxels, &cells[0],0,0,0);
	err |= clEnqueueReadBuffer(queue,sumMem,CL_TRUE,0, sizeof(cl_int), &numberOfCellObjects,0,0,0);


	cellIndices.resize(totalVoxels);

	cellIndices[0] = cells[0]; 


	for (int i = 1; i < totalVoxels; i++)
	{
		cellIndices[i] = cells[i] + cellIndices[i-1];
	}

	cellIncrements.resize(totalVoxels);
	objectIndices.resize(numberOfCellObjects);


	//for(int i = 0; i < objects.size(); i++)
	//{
	//	glm::vec3 cellMin = positionToVoxel(objects[i].box.min  + glm::vec3(objects[i].position),voxelInvWidth, numberOfVoxels,box );
	//	glm::vec3 cellMax = positionToVoxel(objects[i].box.max + glm::vec3(objects[i].position),voxelInvWidth, numberOfVoxels,box );

	//	for(int z = cellMin.z; z <= cellMax.z; z++)
	//	{
	//		for(int y = cellMin.y; y <= cellMax.y; y++)
	//		{
	//			for(int x = cellMin.x; x <= cellMax.x; x++)
	//			{
	//				cells[findVoxelIndex(glm::vec3(x,y,z),numberOfVoxels)]++;
	//				totalVoxels++;
	//			}
	//		}
	//	}
	//}

	cellIndicesMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, cellIndices.size() * sizeof(cl_int),&cellIndices[0], &err);
	objectIndicesMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR, sizeof(cl_int) * numberOfCellObjects, &objectIndices[0], &err );
	cellIncrementsMem = clCreateBuffer(context, CL_MEM_COPY_HOST_PTR, sizeof(cl_int)* totalVoxels, &cellIncrements[0], &err);
	long result = t.elapsed();

}
void RenderWindow::initializeCellObjects()
{
	setUpCellObjectArgs();

	if(err != CL_SUCCESS) {
		perror("Couldn't set the findObjectCells argument");
		exit(1);   
	} 

	err = clEnqueueNDRangeKernel(queue, findObjectCellsKernel, 1 ,
		NULL, &initCellWorkSize, 
		NULL, 0, NULL, NULL);
}
void RenderWindow::initializeDrawScene()
{

	setUpDrawSceneArgs();


	if(err != CL_SUCCESS) {
		perror("Couldn't set the drawSceneKernel argument");
		exit(1);   
	}         
	globalWorkSize[0] = windowWidth;
	globalWorkSize[1] =  windowHeight;

	err = 0;
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
}
void RenderWindow::initializeCL()
{

	//initializeProgram();
	initializeMemory();
	profileTimer.start();
	timer.start();

	float times[5];

	initializeSceneBBox();
	times[0] =  (profileTimer.elapsed()/1000.0f);
	profileTimer.start();
	initializeCells();
	times[1] =  (profileTimer.elapsed()/1000.0f);

	profileTimer.start();
	initializeCellObjects();
	times[2] =  (profileTimer.elapsed()/1000.0f);

	profileTimer.start();
	initializeDrawScene();
	times[3] =  (profileTimer.elapsed()/1000.0f);

	times[4] = timer.elapsed() / 1000.0f;


	cout << "Init SceneBox : " << times[0] << endl;
	cout << "Init Cells : " << times[1] << endl;
	cout << "Init Cell Objects : " << times[2] << endl;
	cout << "Init DrawScene : " << times[3] << endl;
	cout << "Init total : " << times[4] << endl;

}

void RenderWindow::setUpDrawSceneArgs()
{
	err |= clSetKernelArg(drawSceneKernel,0 , sizeof(cl_mem), &clImage);
	err |= clSetKernelArg(drawSceneKernel,1 , sizeof(cl_mem), &writeCLImage);
	err |= clSetKernelArg(drawSceneKernel,2 , sizeof(cl_sampler), &sampler);
	err |= clSetKernelArg(drawSceneKernel,3 , sizeof(cl_int), &windowWidth);
	err |= clSetKernelArg(drawSceneKernel,4 , sizeof(cl_int), &windowHeight);
	err |= clSetKernelArg(drawSceneKernel,5 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(drawSceneKernel,6 , sizeof(cl_mem),(triangles.size())? &trianglesMem : nullptr);
	err |= clSetKernelArg(drawSceneKernel,7 , sizeof(cl_mem), &lightMem);
	err |= clSetKernelArg(drawSceneKernel,8 , sizeof(cl_int), &numberOfObjects);
	err |= clSetKernelArg(drawSceneKernel,9 , sizeof(cl_int), &numberOfLights);
	err |= clSetKernelArg(drawSceneKernel,10, sizeof(BBox), &box);
	err |= clSetKernelArg(drawSceneKernel, 11, sizeof(cl_mem), &cellsMem);
	err |= clSetKernelArg(drawSceneKernel,12 , sizeof(cl_float3), &numberOfVoxels[0]);
	err |= clSetKernelArg(drawSceneKernel, 13, sizeof(cl_mem), &cellIndicesMem);
	err |= clSetKernelArg(drawSceneKernel, 14, sizeof(cl_mem), &objectIndicesMem);
	err |= clSetKernelArg(drawSceneKernel, 15, sizeof(Camera), &camera);
	err |= clSetKernelArg(drawSceneKernel,16 , sizeof(cl_int), &samples);
	err |= clSetKernelArg(drawSceneKernel,17 , sizeof(cl_int), &sampleSquared);
	err |= clSetKernelArg(drawSceneKernel,18 , sizeof(cl_float3) , &delta[0]);
	err |= clSetKernelArg(drawSceneKernel,19 , sizeof(cl_float3) , &deltaInv[0]);
	err |= clSetKernelArg(drawSceneKernel,20 , sizeof(cl_float3) , &voxelInvWidth[0]);
	err |= clSetKernelArg(drawSceneKernel,21 , sizeof(cl_float3) , &numberOfVoxels[0]);
	err |= clSetKernelArg(drawSceneKernel,22 , sizeof(cl_float3) , &voxelWidth[0]);

}
void RenderWindow::setUpCellArgs()
{
	err |= clSetKernelArg(initializeCellsKernel,0 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(initializeCellsKernel,1 , sizeof(cl_mem), &boundingBoxMem);
	err |= clSetKernelArg(initializeCellsKernel,2 , sizeof(cl_mem) , &cellsMem);
	err |= clSetKernelArg(initializeCellsKernel,3 , sizeof(cl_float3), &numberOfVoxels[0]); //checks out
	err |= clSetKernelArg(initializeCellsKernel, 4, sizeof(cl_float3),&voxelInvWidth[0] );
	err |= clSetKernelArg(initializeCellsKernel, 5, sizeof(cl_mem), &sumMem );
}
void RenderWindow::setUpCellObjectArgs()
{
	err |= clSetKernelArg(findObjectCellsKernel,0 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(findObjectCellsKernel,1 , sizeof(cl_mem), &boundingBoxMem);
	err |= clSetKernelArg(findObjectCellsKernel,2 , sizeof(cl_mem) , &cellsMem);
	err |= clSetKernelArg(findObjectCellsKernel,3 , sizeof(cl_mem) , &cellIndicesMem);
	err |= clSetKernelArg(findObjectCellsKernel,4 , sizeof(cl_mem)  , &objectIndicesMem);
	//err |= clSetKernelArg(findObjectCellsKernel,5 , sizeof(cl_mem) , &cellsBoxMem);
	err |= clSetKernelArg(findObjectCellsKernel,5 , sizeof(cl_mem) , &cellIncrementsMem);
	err |= clSetKernelArg(findObjectCellsKernel,6 , sizeof(cl_float3) , &delta[0]);
	err |= clSetKernelArg(findObjectCellsKernel,7 , sizeof(cl_float3) , &deltaInv[0]);
	err |= clSetKernelArg(findObjectCellsKernel,8 , sizeof(cl_float3) , &voxelInvWidth[0]);
	err |= clSetKernelArg(findObjectCellsKernel,9 , sizeof(cl_float3) , &numberOfVoxels[0]); // checks out
}
void RenderWindow::setUpSceneBoxArgs()
{
	err |= clSetKernelArg(sceneBBoxKernel,0 , sizeof(cl_mem), &boundingBoxMem);
	err |= clSetKernelArg(sceneBBoxKernel,1 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(sceneBBoxKernel,2 , sizeof(cl_int), &numberOfObjects);
	err |= clSetKernelArg(sceneBBoxKernel,3 , sizeof(cl_mem),&minMem);
	err |= clSetKernelArg(sceneBBoxKernel,4 , sizeof(cl_mem) , &maxMem);

}


//Grid Helper functions 
//this can't be broken
void RenderWindow::calculateVoxelSize()
{
	delta = box.max - box.min;
	deltaInv = 1.0f / delta;
	float voxelPerUnit = findVoxelsPerUnit();

	for (int i = 0; i < 3; i++)
	{
		numberOfVoxels[i] = (int)(delta[i] * voxelPerUnit);
		numberOfVoxels[i] = glm::clamp((int) numberOfVoxels[i], MIN_VOXELS,MAX_VOXELS);
		voxelWidth[i] = delta[i]/ numberOfVoxels[i];
		voxelInvWidth[i] = (voxelWidth[i] == 0.0f) ? 0.0f : 1.0f / voxelWidth[i];
	}
	//64 57 48 
	totalVoxels = numberOfVoxels[0] * numberOfVoxels[1]* numberOfVoxels[2];

	cout << numberOfVoxels[0] << " , " << numberOfVoxels[1] << ", " <<numberOfVoxels[2] << endl;


	glm::vec3 minPosition = objects[0].box.min + glm::vec3(objects[0].position);
	glm::vec3 maxPosition = objects[0].box.max + glm::vec3(objects[0].position);
	//Testing
	glm::vec3 min = positionToVoxel(minPosition, voxelInvWidth, numberOfVoxels, box);
	glm::vec3 max = positionToVoxel(maxPosition, voxelInvWidth, numberOfVoxels, box);

	int minNumber = findVoxelIndex(min,numberOfVoxels);
	int maxNumber = findVoxelIndex(max,numberOfVoxels);

	glm::vec3 voxPos = findVoxelPosition(minNumber, numberOfVoxels);
	glm::vec3 voxelPosition = voxelToPosition(box,voxPos, voxelWidth);
	glm::vec3 voxelLocation = positionToVoxel(voxelPosition, voxelInvWidth, numberOfVoxels, box);
	int voxelIndex = findVoxelIndex(voxelLocation,numberOfVoxels);

}
float RenderWindow::findVoxelsPerUnit()
{
	int maxAxis = box.maxExtent();
	float invWidth = 1.0f/delta[maxAxis];
	float cubeRoot = 3.0f * pow((float) objects.size(),invWidth);
	return cubeRoot * invWidth;

}