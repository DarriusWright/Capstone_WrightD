__kernel void drawScene(__read_only image2d_t srcImg, __write_only image2d_t dstImage, 
	sampler_t sampler, int width, int height, __global Object * objects, __global Triangle * triangles, 
	__global Light * light, int numberOfObjects, int numberOfLights, BBox box,
	__global int * cells, int nx, int ny, int nz,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera, int samples, int samplesSquared)
{
	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));
	uint4 outColor = (uint4)(0,0,0,255);


		


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
					/*
					for(int i = 0; i < numberOfObjects; i++)
					{
						TriangleInfo triInfo = triangleCollision(ray,triangles[objects[i].triangleIndex]);
								if(triInfo.hasIntersection)
								{
									outColor = (uint4)adsLightT(objects[i], light[0], triInfo);
								}
					}*/


					int3 currentCell;
					float3 cellDimensions = (float3)(nx,ny,nz);
					if(!insideBBox(ray.origin,box.min,box.max ))
					{
						float3 p = ray.origin + (hitCheck.minValue * ray.direction);
						currentCell = (int3)(convert_int3(clamp((p - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), cellDimensions - 1.0f)));

					}
					else
					{
						currentCell = (int3)(convert_int3(clamp((ray.origin - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));
					}
					int cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;

					
					float3 delta = (box.max - box.min)/cellDimensions ;
					float3 next = (convert_float3(currentCell) + 1.0f) * delta;
					bool run =  true;
					float lastDistance = 100000000.0f;
					
					while(run)
					{
						int cellObjectNumber = cellIndices[cellIndex]- cellIndices[cellIndex-1];
						//cellObjectNums[cellIndex] = cellObjectNumber;
						for(int i = 0; i <  cellObjectNumber; i++ )
						{
							int objectIndex = objectIndices[cellIndices[cellIndex-1] + i]-1;

							if(objects[objectIndex].position.w == 0.0)
							{
								TriangleInfo triInfo = triangleCollision(ray,triangles[objects[objectIndex].triangleIndex]);
								if(triInfo.hasIntersection)
								{
									outColor += (uint4)adsLightT(objects[objectIndex], light[0], triInfo);
									lastDistance = triInfo.distanceFromIntersection;
									run = false;
								}

							}
							else
							{
								SphereInfo sphereInfo = sphereIntersection(ray,objects[objectIndex].position.xyz,objects[objectIndex].position.w);
								if(sphereInfo.hasIntersection && sphereInfo.distanceToIntersection < lastDistance)
								{
									outColor += (uint4)adsLightS(objects[objectIndex], light[0], sphereInfo);
									lastDistance = sphereInfo.distanceToIntersection;
									run = false;

								}
							}
						}
						if(run)
						{
							if(next.x  < next.y && next.x < next.z)
							{
								next.x += delta.x;
								currentCell.x += 1;
								run = (currentCell.x != cellDimensions.x);
								
							}
							else if(next.y < next.z)
							{
								next.y += delta.y;
								currentCell.y += 1;
								run = (currentCell.y != cellDimensions.y);
							}
							else
							{
								next.z += delta.z;
								currentCell.z += 1;
								run = (currentCell.z != cellDimensions.z);
							}
							
							
							cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
						}
					}

					
				}

		//	}
		//}
	//outColor /=samples;
	write_imageui(dstImage, outImageCoord, outColor);

}



/*
__kernel void drawScene(__read_only image2d_t srcImg, __write_only image2d_t dstImage, 
	sampler_t sampler, int width, int height, __global Object * objects,
	__global Light * light, int numberOfObjects, int numberOfLights, BBox box,
	__global int * cells, int nx, int ny, int nz,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera
	)
{
	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));
	Ray ray = generateRay(outImageCoord, width, height,camera);
	uint4 outColor = getColor();
HitReturn hitCheck = hitBBox(ray,box.min,box.max);

	if(hitCheck.hit)
	{

		for(int index = 0; index < numberOfObjects; index++)
		{

			if(objects[index].position.w > 0.0)
			{
				SphereInfo sphereInfo = sphereIntersection(ray,objects[index].position.xyz,objects[index].position.w);
				if(sphereInfo.hasIntersection)
				{
					outColor = (uint4)adsLight(objects[index], light[0], sphereInfo);
				}
			}
		}
	}
	write_imageui(dstImage, outImageCoord, outColor);

}

*/