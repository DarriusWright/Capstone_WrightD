typedef struct
{
	float3 nextCrossing;
	float3 deltaT;
	int3 nextStep;
	int3 out;
}StepInfo;

StepInfo findStepInfo(int3 currentCell,Ray ray, float minVal, 
	float maxVal,float3 voxelWidth,BBox box, float3 numberOfVoxels)
{
	
	float3 nextCrossing;
	float3 deltaT;
	int3 nextStep;
	int3 out;

	float3 voxelPositionPos =  voxelToPosition(box, currentCell + 1,voxelWidth.xyz);
	float3 voxelPositionNeg =  voxelToPosition(box, currentCell,voxelWidth.xyz);
	float3 intersectionPoint = ray.origin + ray.direction * minVal;

	if(ray.direction.x >= 0)
	{
		nextCrossing.x = minVal + (voxelPositionPos.x - intersectionPoint.x) / ray.direction.x;
		deltaT.x = voxelWidth.x/ ray.direction.x;
		nextStep.x = 1;
		out.x = numberOfVoxels.x;

	}
	else
	{
		nextCrossing.x = minVal + (voxelPositionNeg.x - intersectionPoint.x) / ray.direction.x;
		deltaT.x= -voxelWidth.x/ ray.direction.x;
		nextStep.x = -1;
		out.x = -1;
	}


	if(ray.direction.y >= 0)
	{
		nextCrossing.y = minVal + (voxelPositionPos.y - intersectionPoint.y) / ray.direction.y;
		deltaT.y = voxelWidth.y/ ray.direction.y;
		nextStep.y = 1;
		out.y = numberOfVoxels.y;

	}
	else
	{
		nextCrossing.y = minVal + (voxelPositionNeg.y- intersectionPoint.y) / ray.direction.y;
		deltaT.y= -voxelWidth.y/ ray.direction.y;
		nextStep.y = -1;
		out.y= -1;
	}

	if(ray.direction.z >= 0)
	{
		nextCrossing.z = minVal + (voxelPositionPos.z- intersectionPoint.z) / ray.direction.z;
		deltaT.z = voxelWidth.z/ ray.direction.z;
		nextStep.z = 1;
		out.z = numberOfVoxels.z;

	}
	else
	{
		nextCrossing.z = minVal + (voxelPositionNeg.z - intersectionPoint.z) / ray.direction.z;
		deltaT.z= -voxelWidth.z/ ray.direction.z;
		nextStep.z = -1;
		out.z = -1;
	}

	StepInfo s;
	s.nextCrossing= nextCrossing;
	s.deltaT = deltaT;
	s.nextStep = nextStep;
	s.out = out;

	return s;	
}

typedef struct
{
	StepInfo stepInfo;
	int3 currentCell;
	bool continueStep;

}StepReturn;


StepReturn stepThroughGrid(StepInfo s, int3 currentCell, float maxVal)
{

	bool run = true;

	int stepAxis =  (s.nextCrossing.x  < s.nextCrossing.y && s.nextCrossing.x < s.nextCrossing.z) ? 0 :(s.nextCrossing.y < s.nextCrossing.z) ? 1 : 2;



	if(stepAxis == 0)
	{


			if(maxVal < s.nextCrossing.x) 
			{
				run = false;
			}
			currentCell.x += s.nextStep.x;

	

			//if((ray.direction.x >= 0) ?  out.x <= currentCell.x : out.x >= currentCell.x)
			if( s.out.x == currentCell.x)
			{
				run = false;

			}
		
			s.nextCrossing.x += s.deltaT.x;

		
	
	}


	if(stepAxis == 1)
	{

			if(maxVal < s.nextCrossing.y)
			{
				run = false;
			}

			currentCell.y += s.nextStep.y;
			//if((ray.direction.y >= 0) ?  out.y <= currentCell.y : out.y >= currentCell.y)
			if( s.out.y == currentCell.y)
			{
				run = false;
			}
			s.nextCrossing.y += s.deltaT.y;

	}


	if(stepAxis == 2)
	{

			if(maxVal < s.nextCrossing.z)
			{
				run = false;
			}

			currentCell.z += s.nextStep.z;
			
			//if(out.z >= currentCell.z)
			if(s.out.z == currentCell.z)
			{
				run = false;
			}
			s.nextCrossing.z += s.deltaT.z;
		
	}


	StepReturn retS;
	retS.stepInfo = s;
	retS.continueStep = run;
	retS.currentCell = currentCell;

	return retS;
}


__kernel void drawScene(__read_only image2d_t srcImg, __write_only image2d_t dstImage, __write_only image2d_t depthBuffer,
	sampler_t sampler, int width, int height, __global Object * objects, __global Triangle * triangles, 
	__global Light * light, int numberOfObjects, int numberOfLights, BBox box,
	__global int * cells, float3 cellDimensions,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera, int samples, int samplesSquared , 
	float3 delta, float3 deltaInv, float3 voxelInvWidth, float3 numberOfVoxels 
	,float3 voxelWidth)

{
	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));
	uint4 outColor = (uint4)(130,45,120,255);
	write_imagef(depthBuffer,outImageCoord,0.0f);


	//	for(int row = 0; row < samplesSquared; row++)
	//	{
	//		for(int column = 0; column < samplesSquared; column++ )
		//	{
				//int2 dim = (int2)(column,row);
				int2 dim = (int2)(0,0);
				Ray ray = generateRay(outImageCoord, width, height, camera,dim,1);
				HitReturn hitCheck = hitBBox(ray,box.min,box.max);

				if(hitCheck.hit)
				{
					int3 currentCell;
					float minVal = 0.0f;
					float maxVal = 0.0f;
					
					if(!insideBBox(ray.origin.xyz,box.min,box.max ))
					{
						float3 p = ray.origin.xyz + (hitCheck.minValue * ray.direction.xyz);
						minVal = hitCheck.minValue;
						maxVal = hitCheck.maxValue;
					    currentCell = (int3)(convert_int3(clamp((p - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), cellDimensions - 1.0f)));
					}
					else
					{
						//currentCell = positionToVoxel(ray.origin, voxelInvWidth, numberOfVoxels-1,box);
						currentCell = (int3)(convert_int3(clamp((ray.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));
						maxVal = hitCheck.maxValue;

					}					

					int cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;

					StepInfo s = findStepInfo(currentCell,ray,minVal, maxVal, voxelWidth, box, numberOfVoxels );
					
					float lastDistance = 10000000.0f;
					bool run = true;

					while(run)
					{
						
						int cellObjectNumber = (cellIndex > 0) ? cellIndices[cellIndex]- cellIndices[cellIndex-1]  : cellIndices[0];
						//if(cellIndex < numberOfVoxels.x * numberOfVoxels.y * numberOfVoxels.z)
						//{
							for(int i = 0; i <  cellObjectNumber && run; i++ )
							{
								int objectIndex = objectIndices[cellIndices[ (cellIndex > 0) ? cellIndex-1 : 0] + i]-1;
									if(objects[objectIndex].position.w == 0.0)
									{
										TriangleInfo triInfo = triangleCollision(ray,triangles[objects[objectIndex].triangleIndex]);
										triInfo.normal = triangles[objects[objectIndex].triangleIndex].normal;
										if(triInfo.hasIntersection)
										{
											outColor = (uint4)adsLightT(objects[objectIndex], light[0], triInfo);
											lastDistance = triInfo.distanceFromIntersection;
											write_imagef(depthBuffer,outImageCoord,lastDistance);
											run = false;
										}

									}

								}

								//if()

						if(run)
						{
							StepReturn stepReturned =  stepThroughGrid(s,currentCell,maxVal);
							currentCell = stepReturned.currentCell;
							run = stepReturned.continueStep;
							s = stepReturned.stepInfo;
						}

						cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
					}
			}
	
	write_imageui(dstImage, outImageCoord, outColor);

}

__kernel void drawShadowRays(__write_only image2d_t dstImage, 
	image2d_t depthBuffer,sampler_t sampler, int width, int height, 
	__global Object * objects, __global Triangle * triangles, 
	__global Light * light, int numberOfObjects, int numberOfLights, 
	BBox box,__global int * cells, float3 cellDimensions,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera, int samples, int samplesSquared , 
	float3 delta, float3 deltaInv, float3 voxelInvWidth, float3 numberOfVoxels 
	,float3 voxelWidth)
{

	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));
	float lastDistance =read_imagef(depthBuffer,sampler,outImageCoord).x;
	if(lastDistance> 0.0f)
	{
		Ray ray = generateRay(outImageCoord, width, height, camera,(int2)(0,0),1);
		HitReturn hitCheck = hitBBox(ray,box.min,box.max);

		uint4 outColor;

		int2 dim = (int2)(0,0);
		bool shadowRun = true;											
		 
		Ray shadowRay;
		shadowRay.origin = (ray.direction.xyz * lastDistance) + ray.origin.xyz;
		shadowRay.direction = normalize(light[0].position - shadowRay.origin);
		HitReturn shadowHit = hitBBox(shadowRay,box.min,box.max);
		int3 currentCell = (int3)(convert_int3(clamp((shadowRay.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));


		StepInfo shadowStep = findStepInfo(currentCell,shadowRay,0.0f, shadowHit.maxValue, voxelWidth, box, numberOfVoxels );

		StepReturn stepRet =  stepThroughGrid(shadowStep,currentCell,shadowHit.maxValue);
		currentCell = stepRet.currentCell;
		shadowStep = stepRet.stepInfo;
		shadowRun = stepRet.continueStep;
		
		int cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;

		while(shadowRun)
		{
			int cellObjectNumber = (cellIndex > 0) ? cellIndices[cellIndex]- cellIndices[cellIndex-1]  : cellIndices[0];

			for(int i = 0; i <  cellObjectNumber && shadowRun; i++ )
			{
				int objectIndex = objectIndices[cellIndices[ (cellIndex > 0) ? cellIndex-1 : 0] + i]-1;

				TriangleInfo triInfo = triangleCollision(shadowRay,triangles[objects[objectIndex].triangleIndex]);
				triInfo.normal = triangles[objects[objectIndex].triangleIndex].normal;
				if(triInfo.hasIntersection)
				{
					outColor = (uint4)(0,0,0,255);
					write_imageui(dstImage, outImageCoord, outColor);
					shadowRun = false;
				}
					
			}
			if(shadowRun)
			{
				StepReturn stepReturned =  stepThroughGrid(shadowStep,currentCell,shadowHit.maxValue);
				currentCell = stepReturned.currentCell;
				shadowRun = stepReturned.continueStep;
				shadowStep = stepReturned.stepInfo;
			}

			cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
			
		}
	}
}

uint3 mixUIntColor(uint3 leftColor, uint3 rightColor)
{
	float3 colorf = convert_float3(leftColor.xyz) / 255.0f;
	float3 colorf2 = convert_float3(rightColor.xyz) / 255.0f;
	float3 finalColor = (colorf * colorf2) * 255.0f;

	return convert_uint3(finalColor);
}


__kernel void drawReflectionRays(__write_only image2d_t dstImage, 
	image2d_t depthBuffer,sampler_t sampler, int width, int height, 
	__global Object * objects, __global Triangle * triangles, 
	__global Light * light, int numberOfObjects, int numberOfLights, 
	BBox box,__global int * cells, float3 cellDimensions,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera, int samples, int samplesSquared , 
	float3 delta, float3 deltaInv, float3 voxelInvWidth, float3 numberOfVoxels 
	,float3 voxelWidth, int numberOfReflections)
{

	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));
	float lastDistance =read_imagef(depthBuffer,sampler,outImageCoord).x;
	if(lastDistance> 0.0f)
	{
		Ray ray = generateRay(outImageCoord, width, height, camera,(int2)(0,0),1);
		HitReturn hitCheck = hitBBox(ray,box.min,box.max);

		int3 currentCell;
		
		if(!insideBBox(ray.origin.xyz,box.min,box.max ))
		{
			float3 p = ray.origin.xyz + (hitCheck.minValue * ray.direction.xyz);
		    currentCell = (int3)(convert_int3(clamp((p - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), cellDimensions - 1.0f)));
		}
		else
		{
			currentCell = (int3)(convert_int3(clamp((ray.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));
		}		

		int cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
		uint4 outColor  = (uint4)(0,0,0,255);
		float3 normal = (float3)(0,0,0);

		int cellObjectNumber = (cellIndex > 0) ? cellIndices[cellIndex]- cellIndices[cellIndex-1]  : cellIndices[0];
		bool findTri = false;
		for(int i = 0; i <  cellObjectNumber && !findTri; i++ )
		{
				int objectIndex = objectIndices[cellIndices[ (cellIndex > 0) ? cellIndex-1 : 0] + i]-1;

				TriangleInfo triInfo = triangleCollision(ray,triangles[objects[objectIndex].triangleIndex]);
				triInfo.normal = triangles[objects[objectIndex].triangleIndex].normal;
				if(triInfo.hasIntersection)
				{
					outColor = (uint4)adsLightT(objects[objectIndex], light[0], triInfo);
					write_imageui(dstImage, outImageCoord, outColor);
					normal = triInfo.normal;
					findTri = true;
				}			
		}
		outColor.w = 255;
		bool reflectionRun = true;											
		float3 reflected = -ray.direction  - 2 * dot(ray.direction , normal) * normal;
		Ray reflectionRay;
		reflectionRay.direction = reflected;

		reflectionRay.origin = ((ray.direction.xyz * lastDistance) + ray.origin.xyz) + (reflected * epsilion);

		HitReturn reflectionHit = hitBBox(reflectionRay,box.min,box.max);
		currentCell = (int3)(convert_int3(clamp((reflectionRay.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));


		StepInfo reflectionStep = findStepInfo(currentCell,reflectionRay,0.0f, reflectionHit.maxValue, voxelWidth, box, numberOfVoxels );
					//StepInfo s = findStepInfo(currentCell,ray,minVal, maxVal, voxelWidth, box, numberOfVoxels );

		StepReturn stepRet =  stepThroughGrid(reflectionStep,currentCell,reflectionHit.maxValue);
		currentCell = stepRet.currentCell;
		reflectionStep = stepRet.stepInfo;
		reflectionRun = stepRet.continueStep;
		
		cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;

		while(reflectionRun)
		{
			int cellObjectNumber = (cellIndex > 0) ? cellIndices[cellIndex]- cellIndices[cellIndex-1]  : cellIndices[0];

			for(int i = 0; i <  cellObjectNumber && reflectionRun; i++ )
			{
				int objectIndex = objectIndices[cellIndices[ (cellIndex > 0) ? cellIndex-1 : 0] + i]-1;

				TriangleInfo triInfo = triangleCollision(reflectionRay,triangles[objects[objectIndex].triangleIndex]);
				triInfo.normal = triangles[objects[objectIndex].triangleIndex].normal;
				if(triInfo.hasIntersection)
				{
					outColor.xyz = mixUIntColor(outColor.xyz,adsLightT(objects[objectIndex], light[0], triInfo).xyz);//convert_uint3(finalColor);//adsLightT(objects[objectIndex], light[0], triInfo).xyz;
					//clamp(outColor, (uint4)(0,0,0,0), (uint4)(255,255,255,255));
					write_imageui(dstImage, outImageCoord, outColor);
					reflectionRun = false;
				}
					
			}
			if(reflectionRun)
			{
				StepReturn stepReturned =  stepThroughGrid(reflectionStep,currentCell,reflectionHit.maxValue);
				currentCell = stepReturned.currentCell;
				reflectionRun = stepReturned.continueStep;
				reflectionStep = stepReturned.stepInfo;
			}

			cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
			
		}
	}
}