
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
		uint4 outColor  = (uint4)(0,100,255,255);
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

		if(findTri)
		{
			bool reflectionRun = true;											
			float3 reflected = ray.direction  - 2 * dot(ray.direction , normal) * normal;
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
					triInfo.normal = triangles[objects[objectIndex].triangleIndex].noral;
					if(triInfo.hasIntersection)
					{
						outColor.xyz = mixUIntColor((uint3)(255,0,255),(uint3)(0,255,0));//adsLightT(objects[objectIndex], light[0], triInfo).xyz);//convert_uint3(finalColor);//adsLightT(objects[objectIndex], light[0], triInfo).xyz;
						//clamp(outColor, (uint4)(0,0,0,0), (uint4)(255,255,255,255));
						//write_imageui(dstImage, outImageCoord, outColor);
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
}