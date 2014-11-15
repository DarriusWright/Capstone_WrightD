float4 radiance(Ray ray, int seed, int maxDepth, int width, 
	int height, __global Object * objects, __global Triangle * triangles, 
	__global Light * light, BBox box,__global int * cells, 
	float3 cellDimensions,__global int * cellIndices, __global int * objectIndices,
	Camera camera, int samples, int samplesSquared , float3 delta, float3 deltaInv, 
	float3 voxelInvWidth, float3 numberOfVoxels ,float3 voxelWidth ,
	__local float3 * levelColor, __local int * oIndices , __local float3 * emissions)
{
	//default depth to 0
	int depth = 0;
	// initialize color to black
	float3 color = (float3)(0,0,0);

	//float3 radiance = (float3)(0,0,0);

	//does the ray hit the scene bounding box?
	HitReturn hitCheck = hitBBox(ray,box.min,box.max);
	if(hitCheck.hit)
	{
		//find the current cell and the current cell index 
		int cellIndex;
		float minVal = 0.0f;
		float maxVal = 0.0f;
		int3 currentCell;
		
		if(!insideBBox(ray.origin.xyz,box.min,box.max ))
		{
			float3 p = ray.origin.xyz + (hitCheck.minValue * ray.direction.xyz);
			minVal = hitCheck.minValue;
			maxVal = hitCheck.maxValue;
		    currentCell = (int3)(convert_int3(clamp((p - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), cellDimensions - 1.0f)));
		}
		else
		{
			currentCell = (int3)(convert_int3(clamp((ray.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));
			maxVal = hitCheck.maxValue;

		}	
		cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
		
		//find the inttersection in the scene
		IntersectionInfo intersect  = rayTrace(ray, camera,cellIndices, objectIndices,light,
				objects, triangles, cellIndex, currentCell, box,voxelWidth,numberOfVoxels);

		// if there is no intersection return black
		if(intersect.distance == -1) return (float4)(0,0,0,1);
		

		//get the objects information
		//find the normal of the object 
		float3 normal =  triangles[objects[intersect.objectIndex].triangleIndex].normal;
		//is it inside the object, if so inverse the normal
		float3 orientedNormal = dot(normal, ray.direction) < 0 ? normal : normal*-1;
		float3 objectColor = objects[intersect.objectIndex].material.diffuse.xyz;
		Object object = objects[intersect.objectIndex];
		//emission of the light 
		float3 emission = (float3)(400,400,400); // this is temporary...
		
		//as long as the depth has not reached the max continue 
		while(depth < maxDepth)
		{
			depth++; // first increment the depth
			float p = max(max(objectColor.x,objectColor.y), objectColor.z); // find the max color
			
			int colorIndex = maxDepth - depth;
			// russian roulette
			if(depth > 5 || !p) 
			{
				if(randFloat(seed + depth) < p) 
				{
					objectColor *=(1/p);
				}
				else
				{
					levelColor[colorIndex] = (float3)(0,0,0); //have objects support irradance;
					oIndices[colorIndex] = intersect.objectIndex; 
					continue;
				}
			}
			//diffuse?
			if(objects[intersect.objectIndex].material.type == DIFFUSE)
			{
				seed += depth;
				float r1 = 2 * PI * randFloat(seed); // random angle 
				seed += depth;
				float r2 = randFloat(seed + cellIndex); // random distance 
				float r2s = sqrt(r2);
				float3 w = orientedNormal;
				float3 u =normalize(cross((absF(w.x) > .1 ? (float3)(0,1,0) : (float3)(1,0,0)),w));
				float3 v = cross(w,u);
				float3 direction = normalize(u * cos(r1) * r2s  + v * sin(r1) * r2s + w * sqrt(1 -r2));
				
				float3 e = (float3)(0,0,0);
				float3 sw = light[0].position.xyz - intersect.position;
				float3 su = normalize(cross(absF(sw.x) > .1 ? (float3)(0,1,0) : (float3)(1,0,0),sw));
				float3 sv = cross(sw,su);
				float cos_a_max =sqrt(1 -  light[0].direction.w * light[0].direction.w/dot((intersect.position - light[0].position),(intersect.position - light[0].position) ) );
				float eps1 = randFloat(seed);
				float eps2 = randFloat(seed + seed);
				float cos_a = 1 - eps1 + eps1 * cos_a_max;
				float sin_a = sqrt(1 - cos_a * cos_a);
				float  phi = 2 * PI * eps2;
				float3 lightVector = -su * cos(phi) * sin_a + -sv * sin(phi) * sin_a + sw * cos_a;
				lightVector = normalize(lightVector);

				Ray shadowRay;
				shadowRay.origin = intersect.position;
				shadowRay.direction = lightVector;

				int objectIndex = intersect.objectIndex;

				IntersectionInfo shadowIntersection = rayTrace(shadowRay, camera,cellIndices, objectIndices,light,
				objects, triangles, cellIndex, currentCell, box,voxelWidth,numberOfVoxels);

				if(shadowIntersection.distance != -1.0f && objectIndex == shadowIntersection.objectIndex)
				{
					float omega = 2.0f * PI * (1.0f - cos_a_max);
					e =  objectColor * (emission * dot(lightVector, orientedNormal) * omega) * PI_1 ;
				}


				ray.origin.xyz = intersect.position.xyz;
				ray.direction = direction;
				seed += seed;

				emissions[colorIndex] = e;
				oIndices[colorIndex] = intersect.objectIndex;
				levelColor[colorIndex] = objectColor;
				//return e + objectColor * (emission * dot(lightVector, orientedNormal) );
				intersect  = rayTrace(ray, camera,cellIndices, objectIndices,light,
				objects, triangles, cellIndex, currentCell, box,voxelWidth,numberOfVoxels);
				

				//return (float4)(objectColor,1);
				if(intersect.distance == -1) break;
				continue;
			}
			else if(objects[intersect.objectIndex].material.type == SPECULAR)
			{

				ray.origin = intersect.position;
				ray.direction = ray.direction - normal * 2.0f * dot(normal,ray.direction);
				
				oIndices[maxDepth - depth] = intersect.objectIndex;
				levelColor[maxDepth - depth] = (float3)(0,0,0);// e + objectColor * (emission * dot(lightVector, orientedNormal) );
				continue;
			}



			break;
		}
	
	}

	//for(
	if(depth > 0)
	{
		for(int i = maxDepth - depth; i < maxDepth; i++)
		{
			float3 objectColor = objects[oIndices[i]].material.diffuse.xyz;
			color += emissions[i] +(color + levelColor[i]);
			//float3 obColor = objects[oIndices[maxDepth - 1]].material.diffuse.xyz;
			//color = obColor;
		}
	}
	
	return (float4)(color,1);
}

float random(float3 scale, float seed,float3 imageCoord) {	
	float randomNum =sin(dot(imageCoord.xyz + seed, scale)) * 43758.5453f + seed; 
	return randomNum - (int)randomNum;	
}	


//gives a uniform random direction vector
float3 uniformlyRandomDirection(float seed,float3 imageCoord) {		
	float u = random((float3)(12.9898, 78.233, 151.7182), seed, imageCoord);		
	float v = random((float3)(63.7264, 10.873, 623.6736), seed, imageCoord);		
	float z = 1.0 - 2.0 * u;		
	float r = sqrt(1.0 - z * z);	
	float angle = 6.283185307179586 * v;	
	return (float3)(r * cos(angle), r * sin(angle), z);	
}	

//returns a uniformly random vector
float3 uniformlyRandomVector(float seed, float3 imageCoord) 
{		
	return uniformlyRandomDirection(seed, imageCoord) *  (random((float3)(36.7539, 50.3658, 306.2759), seed,imageCoord));	
}


float3 pathTrace(Ray ray,__global Light * light,float t , float4 backgroundColor,int MAX_BOUNCES, BBox box, int seed,  int width, 
	int height, __global Object * objects, __global Triangle * triangles, 
	__global int * cells, float3 cellDimensions,__global int * cellIndices, __global int * objectIndices,
	Camera camera, float3 delta, float3 deltaInv, float3 voxelInvWidth, float3 numberOfVoxels ,
	float3 voxelWidth , float3 imageCoord , float3 lightVector)
{
	float3 mask = (float3)(1,1,1);
	float3 colorSum = (float3)(0,0,0);
	float3 surfaceColor = backgroundColor.xyz;

	float diffuse = 1;

	for(int bounce = 0; bounce < MAX_BOUNCES; bounce++)
	{
		HitReturn hitRet = hitBox(ray,box);

		if(hitRet.minValue > hitRet.maxValue)
			continue;
		if(hitRet.maxValue < t)
			t = hitRet.maxValue + 1;

		
		//float4 val = (float4)(t,0,0,0); 

		int cellIndex;
		float minVal = 0.0f;
		float maxVal = 0.0f;
		int3 currentCell;
		
		if(!insideBBox(ray.origin.xyz,box.min,box.max ))
		{
			float3 p = ray.origin.xyz + (hitRet.minValue * ray.direction.xyz);
			minVal = hitRet.minValue;
			maxVal = hitRet.maxValue;
		    currentCell = (int3)(convert_int3(clamp((p - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), cellDimensions - 1.0f)));
		}
		else
		{
			currentCell = (int3)(convert_int3(clamp((ray.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));
			maxVal = hitRet.maxValue;

		}	
		cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
		
		float3 normal;

		//find the inttersection in the scene
		IntersectionInfo intersect  = rayTrace(ray, camera,cellIndices, objectIndices,light,
				objects, triangles, cellIndex, currentCell, box,voxelWidth,numberOfVoxels);
		
		if(intersect.distance == -1) break;

		normal = triangles[objects[intersect.objectIndex].triangleIndex].normal;

		if(intersect.distance < t)
		{
			surfaceColor = objects[intersect.objectIndex].material.diffuse.xyz;
			float3 hit = intersect.position;
			ray.origin = hit;
			ray.direction = uniformlyRandomDirection(seed + (float)(bounce), imageCoord);
			float3 jitteredLight = lightVector + ray.direction;
			float3 L = normalize(jitteredLight - hit);
			diffuse = max(0.0f, dot(L,normal));
			mask *= surfaceColor;

			Ray shadowRay;
			shadowRay.direction = L;
			shadowRay.origin = hit;

			IntersectionInfo shadowIntersect  = rayTrace(shadowRay, camera,cellIndices, objectIndices,light,
			objects, triangles, cellIndex, currentCell, box,voxelWidth,numberOfVoxels);
	

			float shadowMult = shadowIntersect.distance == -1.0f ? 1.0f : 0.5f;

			colorSum  += mask * diffuse * shadowMult;
			t = intersect.distance;

		}

	}

	return (colorSum == (float3)(0,0,0)) ? surfaceColor * diffuse : colorSum / (float)(MAX_BOUNCES-1);


}

__kernel void drawScene(__read_only image2d_t srcImg, __write_only image2d_t dstImage, __write_only __global float * depthBuffer,
	sampler_t sampler, int width, int height, __global Object * objects, __global Triangle * triangles, 
	__global Light * light, int numberOfObjects, int numberOfLights, BBox box,
	__global int * cells, float3 cellDimensions,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera, int samples, int numberOfSamples , 
	float3 delta, float3 deltaInv, float3 voxelInvWidth, float3 numberOfVoxels 
	,float3 voxelWidth, int seed, int MAX_BOUNCES,
	 int shadowsEnabled, int reflectionsEnabled, int refractionsEnabled,float4 backgroundColor)
{
	uint4 outColor;
	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));
	float3 imageCoord = (float3)(get_global_id(0),get_global_id(1),1.0f);
	float4 color =(float4)(0,0,0,1);//backgroundColor;
	float4 radiantColor = (float4)(0,0,0,0);
	float inverseSamples = 1.0f ;
	for(int sy = 0; sy < 2; sy++ )
	{
		for(int sx = 0; sx < 2; sx++ )
		{
			float3 radiantColor = (float3)(0,0,0);
			
			for(int sam = 0; sam < numberOfSamples; sam++)
			{
				Ray ray= generateRay(outImageCoord, width, height,camera,(int2)(sx,sy), numberOfSamples, seed);
				HitReturn hitRet = hitBox(ray,box);
				if(hitRet.hit)
				{
					float3 lightVector = light[0].position + uniformlyRandomVector(seed , imageCoord);
					radiantColor += (float4)(pathTrace(ray,light,hitRet.maxValue, backgroundColor,MAX_BOUNCES, box,  seed,   width, 
	 										height, objects, triangles, cells, cellDimensions,cellIndices, objectIndices,camera,  delta,  
											 deltaInv, voxelInvWidth,  numberOfVoxels , voxelWidth, imageCoord, lightVector),0.0f) * inverseSamples;
				}
			}
			color.xyz += clamp(radiantColor.xyz, (float3)(0,0,0), (float3)(1,1,1)) * 0.25f;
		}
	}


	outColor = toRGBA(color);
	write_imageui(dstImage, outImageCoord, outColor);

}


/*
__kernel void drawScene(__read_only image2d_t srcImg, __write_only image2d_t dstImage, __write_only __global float * depthBuffer,
	sampler_t sampler, int width, int height, __global Object * objects, __global Triangle * triangles, 
	__global Light * light, int numberOfObjects, int numberOfLights, BBox box,
	__global int * cells, float3 cellDimensions,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera, int samples, int samplesSquared , 
	float3 delta, float3 deltaInv, float3 voxelInvWidth, float3 numberOfVoxels 
	,float3 voxelWidth, int seed, int maxDepth,__local float3 * emissions, __local float3 * levelColor, __local int * objectIndex,
	 int shadowsEnabled, int reflectionsEnabled, int refractionsEnabled)

{
	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));
	uint4 outColor = (uint4)(0,0,0,255);
	float4 color;
	color.xyz = (float3)(0,0,0);
	color.w = 1.0f;

	//write_imagef(depthBuffer,outImageCoord,0.0f);
	float inverseSamples = 1.0f ;
	for(int sy = 0; sy < 2; sy++ )
	{
		for(int sx = 0; sx < 2; sx++ )
		{
			float3 radiantColor = (float3)(0,0,0);
			int sam = 1;
			for(int sam = 0; sam < 1; sam++)
			{
				Ray ray= generateRay(outImageCoord, width, height,camera,(int2)(sx,sy), samplesSquared, seed);
				//radiantColor = (float3)(1,1,1);
				radiantColor = radiance(ray,seed + outImageCoord.x + outImageCoord.y + sam + sx + sy, maxDepth,   width,  height, objects, triangles, light, box,cells, cellDimensions,
				cellIndices, objectIndices, camera, samples, samplesSquared , delta, deltaInv, voxelInvWidth, numberOfVoxels , voxelWidth, levelColor, objectIndex , emissions).xyz * inverseSamples;
			}

			color.xyz += clamp(radiantColor, (float3)(0,0,0), (float3)(1,1,1)) * 0.25f;
		}
	}


	outColor = toRGBA(color);
	write_imageui(dstImage, outImageCoord, outColor);

}
*/
/*
__kernel void drawScene(__read_only image2d_t srcImg, __write_only image2d_t dstImage, __write_only __global float * depthBuffer,
	sampler_t sampler, int width, int height, __global Object * objects, __global Triangle * triangles, 
	__global Light * light, int numberOfObjects, int numberOfLights, BBox box,
	__global int * cells, float3 cellDimensions,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera, int samples, int samplesSquared , 
	float3 delta, float3 deltaInv, float3 voxelInvWidth, float3 numberOfVoxels 
	,float3 voxelWidth, int seed, int shadowsEnabled, int reflectionsEnabled, int refractionsEnabled)

{
	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));
	uint4 outColor = (uint4)(0,0,0,255);
	//write_imagef(depthBuffer,outImageCoord,0.0f);


		for(int row = 0; row < samplesSquared; row++)
		{
			for(int column = 0; column < samplesSquared; column++ )
			{
				int depthBufferSampleLocation = ((outImageCoord.y * width * samples) + (outImageCoord.x * samples)  ) + ((row * samplesSquared) + column);
				depthBuffer[ depthBufferSampleLocation] =  0.0f;
				int2 dim = (int2)(row,column);
				//int2 dim = (int2)(0,0);
				Ray ray = generateRay(outImageCoord, width, height, camera,dim,samples, seed);
				HitReturn hitCheck = hitBBox(ray,box.min,box.max);

				if(hitCheck.hit)
				{
					int3 currentCell;
					float minVal = 0.0f;
					float maxVal = 0.0f;
					float lastDistance = 10000000.0f;

					
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
					
					bool run = true;

					while(run)
					{
						
						int cellObjectNumber = (cellIndex > 0) ? cellIndices[cellIndex]- cellIndices[cellIndex-1]  : cellIndices[0];
						int objectColorIndex = 0;
						float3 normal;
						for(int i = 0; i <  cellObjectNumber; i++ )
						{
							int objectIndex = objectIndices[cellIndices[ (cellIndex > 0) ? cellIndex-1 : 0] + i]-1;
							if(objects[objectIndex].position.w == 0.0)
							{
								TriangleInfo triInfo = triangleCollision(ray,triangles[objects[objectIndex].triangleIndex]);
								triInfo.normal = triangles[objects[objectIndex].triangleIndex].normal;
								if(triInfo.hasIntersection && lastDistance  > triInfo.distanceFromIntersection)
								{
									//outColor = (uint4)adsLight(objects[objectIndex], light[0], triInfo);
									lastDistance = triInfo.distanceFromIntersection;
									normal = triInfo.normal;
									objectColorIndex = objectIndex;
									run = false;
								}

							}

						}

						if(!run)
						{
							uint4 lightColor = adsLight(light[0], objects[objectColorIndex].material, camera, ray.direction * lastDistance, normal);
							outColor +=  lightColor;
							depthBuffer[depthBufferSampleLocation] = lastDistance;
						}

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
		}
	}
	outColor.xyz/= samples;
	outColor.w = 255;
	write_imageui(dstImage, outImageCoord, outColor);

}
*/


__kernel void drawShadowRays(__write_only image2d_t dstImage, 
	__global float * depthBuffer,sampler_t sampler, int width, int height, 
	__global Object * objects, __global Triangle * triangles, 
	__global Light * light, int numberOfObjects, int numberOfLights, 
	BBox box,__global int * cells, float3 cellDimensions,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera, int samples, int samplesSquared , 
	float3 delta, float3 deltaInv, float3 voxelInvWidth, float3 numberOfVoxels 
	,float3 voxelWidth , int seed)
{

	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));


		bool shadowLocated = false;
		uint4 outColor;// = (uint4)(0,0,0,255);

		for(int row = 0; row < samplesSquared; row++)
		{
			for(int column = 0; column < samplesSquared; column++ )
			{
				int depthBufferSampleLocation = ((outImageCoord.y * width * samples) + (outImageCoord.x * samples)  ) + ((row * samplesSquared) + column);

				float lastDistance = depthBuffer[depthBufferSampleLocation];

				if(lastDistance> 0.0f)
				{

				int2 dim = (int2)(row,column);
				//int2 dim = (int2)(0,0);
				
				
				Ray ray = generateRay(outImageCoord, width, height, camera,dim,samples , seed );
				HitReturn hitCheck = hitBBox(ray,box.min,box.max);
				bool shadowRun = true;											
				 
				Ray shadowRay;
				shadowRay.origin = (ray.direction.xyz * lastDistance) + ray.origin.xyz;
				shadowRay.direction = normalize(light[0].position - shadowRay.origin);
				shadowRay.origin += (shadowRay.direction * epsilion);
				HitReturn shadowHit = hitBBox(shadowRay,box.min,box.max);
				//int3 currentCell = (int3)(convert_int3(clamp((shadowRay.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));
					int3 currentCell;
					float minVal = 0.0f;
					float maxVal = 0.0f;
					
					if(!insideBBox(shadowRay.origin.xyz,box.min,box.max ))
					{
						float3 p = shadowRay.origin.xyz + (hitCheck.minValue * shadowRay.direction.xyz);
						minVal = hitCheck.minValue;
						maxVal = hitCheck.maxValue;
					    currentCell = (int3)(convert_int3(clamp((p - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), cellDimensions - 1.0f)));
					}
					else
					{
						//currentCell = positionToVoxel(ray.origin, voxelInvWidth, numberOfVoxels-1,box);
						currentCell = (int3)(convert_int3(clamp((shadowRay.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));
						maxVal = hitCheck.maxValue;

					}	

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
				//write_imageui(dstImage, outImageCoord, outColor);
							
							shadowRun = false;
							shadowLocated = true;
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


		if(shadowLocated)
		{
			outColor.xyz/=samples;
			write_imageui(dstImage, outImageCoord, outColor);
		}
	}
}



__kernel void drawReflectionRays(__write_only image2d_t dstImage, 
	__global float * depthBuffer,sampler_t sampler, int width, int height, 
	__global Object * objects, __global Triangle * triangles, 
	__global Light * light, int numberOfObjects, int numberOfLights, 
	BBox box,__global int * cells, float3 cellDimensions,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera, int samples, int samplesSquared , 
	float3 delta, float3 deltaInv, float3 voxelInvWidth, float3 numberOfVoxels 
	,float3 voxelWidth, int numberOfReflections, int seed)
{
	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));
	uint4 outColor = (uint4)(0,0,0,255);
	bool reflectionLocated = false;


		for(int row = 0; row < samplesSquared; row++)
		{

			for(int column = 0; column < samplesSquared; column++ )
			{
				int depthBufferSampleLocation = ((outImageCoord.y * width * samples) + (outImageCoord.x * samples)  ) + ((row * samplesSquared) + column);
				float lastDistance = depthBuffer[depthBufferSampleLocation];
				Ray ray = generateRay(outImageCoord, width, height, camera,(int2)(row,column),samples , seed);


				for(int i = 0; i < numberOfReflections; i++)
				{ 
					bool reflectedOnObject = false;

				if(lastDistance> 0.0f)
				{



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
							outColor = adsLight(light[0], objects[objectIndex].material, camera, ray.direction * triInfo.distanceFromIntersection, triInfo.normal);
							normal = triInfo.normal;
							findTri = true;
						}			
				}
				outColor.w = 255;
				bool reflectionRun = true;											
				float3 reflected =-ray.direction  - 2 * dot(ray.direction , normal) * normal;
				Ray reflectionRay;
				reflectionRay.direction = reflected;
				reflectionRay.origin = ((ray.direction.xyz * lastDistance) + ray.origin.xyz)+ (reflected * epsilion);

					HitReturn reflectionHit = hitBBox(reflectionRay,box.min,box.max);
					currentCell = (int3)(convert_int3(clamp((reflectionRay.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));


					StepInfo reflectionStep = findStepInfo(currentCell,reflectionRay,0.0f, reflectionHit.maxValue, voxelWidth, box, numberOfVoxels );
								//StepInfo s = findStepInfo(currentCell,ray,minVal, maxVal, voxelWidth, box, numberOfVoxels );

					StepReturn stepRet =  stepThroughGrid(reflectionStep,currentCell,reflectionHit.maxValue);
					currentCell = stepRet.currentCell;
					reflectionStep = stepRet.stepInfo;
					reflectionRun = stepRet.continueStep;
					
					cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
					int reflectedNumber = 0;
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
								outColor.xyz += mixUIntColor(outColor.xyz,adsLight(light[0], objects[objectIndex].material, camera, reflectionRay.direction * triInfo.distanceFromIntersection, triInfo.normal).xyz);//convert_uint3(finalColor);//adsLightT(objects[objectIndex], light[0], triInfo).xyz;
								//clamp(outColor, (uint4)(0,0,0,0), (uint4)(255,255,255,255));
								//outColor = adsLight(light[0], objects[objectIndex].material, camera, reflectionRay.direction * triInfo.distanceFromIntersection, triInfo.normal);
								
								lastDistance = triInfo.distanceFromIntersection;
								reflectionRun = false;
								reflectionLocated = true;
								reflectedOnObject = true;
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
					if(reflectedOnObject)
					{
						float3 reflected =-reflectionRay.direction  - 2 * dot(reflectionRay.direction , normal) * normal;
						ray.origin = (lastDistance * reflectionRay.direction) + reflectionRay.origin;
						ray.direction = reflected;
						ray.origin +=  (reflected * epsilion);
					}
					//redo ray stuff
				}
			}
		}
	}
	if(reflectionLocated)
	{
		outColor.xyz/= samples;
	//	outColor.xyz/= numberOfReflections;
		write_imageui(dstImage, outImageCoord, outColor);
	}
}

__kernel void drawRefractionRays(__write_only image2d_t dstImage, 
	__global float * depthBuffer,sampler_t sampler, int width, int height, 
	__global Object * objects, __global Triangle * triangles, 
	__global Light * light, int numberOfObjects, int numberOfLights, 
	BBox box,__global int * cells, float3 cellDimensions,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera, int samples, int samplesSquared , 
	float3 delta, float3 deltaInv, float3 voxelInvWidth, float3 numberOfVoxels 
	,float3 voxelWidth, int numberOfReflections, int seed )
{

	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));
				uint4 outColor = (uint4)(0,0,0,255);
		bool refractionLocated  = false;
		for(int row = 0; row < samplesSquared; row++)
		{
			for(int column = 0; column < samplesSquared; column++ )
			{

				int depthBufferSampleLocation = ((outImageCoord.y * width * samples) + (outImageCoord.x * samples)  ) + ((row * samplesSquared) + column);
				float lastDistance = depthBuffer[depthBufferSampleLocation];

				if(lastDistance> 0.0f)
				{


				Ray ray = generateRay(outImageCoord, width, height, camera,(int2)(row,column),samples , seed);

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
				float3 normal = (float3)(0,0,0);

				int cellObjectNumber = (cellIndex > 0) ? cellIndices[cellIndex]- cellIndices[cellIndex-1]  : cellIndices[0];
				bool findTri = false;
				int objectIndex = 0;
				float oDistance = 0.0f;
				for(int i = 0; i <  cellObjectNumber && !findTri; i++ )
				{
						objectIndex = objectIndices[cellIndices[ (cellIndex > 0) ? cellIndex-1 : 0] + i]-1;

						TriangleInfo triInfo = triangleCollision(ray,triangles[objects[objectIndex].triangleIndex]);
						triInfo.normal = triangles[objects[objectIndex].triangleIndex].normal;
						if(triInfo.hasIntersection)
						{
							outColor = adsLight(light[0], objects[objectIndex].material, camera, ray.direction * triInfo.distanceFromIntersection, triInfo.normal);
							normal = triInfo.normal;
							oDistance = triInfo.distanceFromIntersection;
							findTri = true;
						}			
				}
				//outColor.w = 255;
				bool refractionRun = true;											
				float3 lightDirection = (light[0].type == DIRECTIONAL_LIGHT) ? light[0].direction.xyz : light[0].position.xyz - (ray.direction.xyz * oDistance);
				float angle  = -dot(-ray.direction, normal);
				float n1 = airIndex;
				float n2 = 1.46f;//objects[objectIndex].material.refraction;

				float index = n1/n2;

				float refractedAngle = sqrt(1- (index*index) * (1 - (angle * angle)));
				//float refractedAngle = (index*index) * (1 - (angle * angle));
				if(refractedAngle>= 0.0f && findTri)
				{

					float3 refracted = (index * (angle - refractedAngle))  * - normal - (index * -ray.direction);
					Ray refractionRay;
					refractionRay.direction = normalize(refracted);

					refractionRay.origin = ((ray.direction.xyz * lastDistance) + ray.origin.xyz) + (refracted * epsilion);

					HitReturn refractionHit = hitBBox(refractionRay,box.min,box.max);
					currentCell = (int3)(convert_int3(clamp((refractionRay.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));


					StepInfo refractionStep = findStepInfo(currentCell,refractionRay,0.0f, refractionHit.maxValue, voxelWidth, box, numberOfVoxels );
				
					StepReturn stepRet =  stepThroughGrid(refractionStep,currentCell,refractionHit.maxValue);
					currentCell = stepRet.currentCell;
					refractionStep = stepRet.stepInfo;
					refractionRun = stepRet.continueStep;
							
					cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
				
					while(refractionRun)
					{
						int cellObjectNumber = (cellIndex > 0) ? cellIndices[cellIndex]- cellIndices[cellIndex-1]  : cellIndices[0];

						for(int i = 0; i <  cellObjectNumber && refractionRun; i++ )
						{
							int objectIndex = objectIndices[cellIndices[ (cellIndex > 0) ? cellIndex-1 : 0] + i]-1;

							TriangleInfo triInfo = triangleCollision(refractionRay,triangles[objects[objectIndex].triangleIndex]);
							triInfo.normal = triangles[objects[objectIndex].triangleIndex].normal;
							if(triInfo.hasIntersection)
							{
								///float frenselEffect = mix(pow(1 - dot(ray.direction.xyz,normal), 3) ,1,0.1);
								//outColor.xyz = mixUIntColor(convert_uint3(convert_float3(outColor.xyz) * (1 - frenselEffect) * 0.5f), convertColor(objects[objectIndex].material.diffuse)); 
								outColor.xyz = mixUIntColor(outColor.xyz,adsLight(light[0], objects[objectIndex].material, camera, refractionRay.direction * triInfo.distanceFromIntersection, triInfo.normal).xyz);//convert_uint3(finalColor);//adsLightT(objects[objectIndex], light[0], triInfo).xyz;
								refractionRun = false;//(numberOfIter < 2);
								refractionLocated = true;
							}
								
						}
						if(refractionRun)
						{
							StepReturn stepReturned =  stepThroughGrid(refractionStep,currentCell,refractionHit.maxValue);
							currentCell = stepReturned.currentCell;
							refractionRun = stepReturned.continueStep;
							refractionStep = stepReturned.stepInfo;
						}

						cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
						
					}
				}
			}
		}
		if(refractionLocated)
		{
			outColor.xyz/= samples;
			write_imageui(dstImage, outImageCoord, outColor);
		}

	}
}

