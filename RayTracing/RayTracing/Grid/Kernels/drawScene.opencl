__kernel void drawScene(__read_only image2d_t srcImg, __write_only image2d_t dstImage, 
	sampler_t sampler, int width, int height, __global Object * objects, __global Triangle * triangles, 
	__global Light * light, int numberOfObjects, int numberOfLights, BBox box,
	__global int * cells, float3 cellDimensions,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera, int samples, int samplesSquared , 
	float3 delta, float3 deltaInv, float3 voxelInvWidth, float3 numberOfVoxels 
	,float3 voxelWidth)

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
						currentCell = (int3)(convert_int3(clamp((ray.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));
						maxVal = hitCheck.maxValue;

					}
					
					float3 intersectionPoint = ray.origin.xyz + (ray.direction.xyz * minVal);

					int cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
					float3 nextCrossing;
					float3 deltaT;
					int3 step;
					int3 out;
				


					if(ray.direction.x > 0)
					{
						nextCrossing.x = minVal + (voxelToPosition(box, currentCell + 1,voxelWidth.xyz).x- (minVal * ray.direction + ray.origin).x) / ray.direction.x;
						deltaT.x = voxelWidth.x/ ray.direction.x;
						step.x = 1;
						out.x = numberOfVoxels.x;

					}
					else
					{
						nextCrossing.x = minVal + (voxelToPosition(box, currentCell,voxelWidth.xyz).x- (minVal * ray.direction + ray.origin).x) / ray.direction.x;
						deltaT.x= -voxelWidth.x/ ray.direction.x;
						step.x = -1;
						out.x = -1;
					}


					if(ray.direction.y > 0)
					{
						nextCrossing.y = minVal + (voxelToPosition(box, currentCell + 1,voxelWidth.xyz).y - (minVal * ray.direction + ray.origin).y) / ray.direction.y;
						deltaT.y = voxelWidth.y/ ray.direction.y;
						step.y = 1;
						out.y = numberOfVoxels.y;

					}
					else
					{
						nextCrossing.y = minVal + (voxelToPosition(box, currentCell,voxelWidth.xyz).y- (minVal * ray.direction +ray.origin).y) / ray.direction.y;
						deltaT.y= -voxelWidth.y/ ray.direction.y;
						step.y = -1;
						out.y= -1;
					}

					if(ray.direction.z > 0)
					{
						nextCrossing.z = minVal + (voxelToPosition(box, currentCell + 1,voxelWidth.xyz).z- (minVal * ray.direction + ray.origin).z) / ray.direction.z;
						deltaT.z = voxelWidth.z/ ray.direction.z;
						step.z = 1;
						out.z = numberOfVoxels.z;

					}
					else
					{
						nextCrossing.z = minVal + (voxelToPosition(box, currentCell,voxelWidth.xyz).z - (minVal * ray.direction + ray.origin).z) / ray.direction.z;
						deltaT.z= -voxelWidth.z/ ray.direction.z;
						step.z = -1;
						out.z = -1;
					}

					int bits =0;
					float lastDistance = 10000000.0f;
					while(true)
					{
						int cellObjectNumber = cellIndices[cellIndex]- cellIndices[cellIndex-1];

						for(int i = 0; i <  cellObjectNumber; i++ )
						{
							int objectIndex = objectIndices[cellIndices[cellIndex-1] + i]-1;

							if(objects[objectIndex].position.w == 0.0)
							{
								TriangleInfo triInfo = triangleCollision(ray,triangles[objects[objectIndex].triangleIndex]);
								if(triInfo.hasIntersection)
								{
									outColor = (uint4)adsLightT(objects[objectIndex], light[0], triInfo);
									lastDistance = triInfo.distanceFromIntersection;									
									//run = false;
									break;
								}

							}
							else
							{
								SphereInfo sphereInfo = sphereIntersection(ray,objects[objectIndex].position.xyz,objects[objectIndex].position.w);
								if(sphereInfo.hasIntersection && sphereInfo.distanceToIntersection < lastDistance)
								{
									outColor = (uint4)adsLightS(objects[objectIndex], light[0], sphereInfo);
									lastDistance = sphereInfo.distanceToIntersection;									
									//run = false;
									break;
								}
							}
						}

						bits = ((nextCrossing.x < nextCrossing.y) << 2) + ((nextCrossing.x < nextCrossing.z) << 1) + (nextCrossing.y < nextCrossing.z);
                        const int cmpAxis[8] = {2,1,2,1,2,2,0,0};
						int stepAxis = cmpAxis[bits];

						if(stepAxis == 0)
						{

							if(maxVal < nextCrossing.x) 
							{
								break;
							}
						}
						if(stepAxis == 1)
						{

							if(maxVal < nextCrossing.y)
							{
								break;
							}
						}
						if(stepAxis == 2)
						{
							if(maxVal < nextCrossing.z)
							{
								break;
							}
						}


						if(stepAxis == 0)
						{
							currentCell.x += step.x;

							if( out.x == currentCell.x)
							{
								break;
							}

							nextCrossing.x += deltaT.x;

						}
						if(stepAxis == 1)
						{
							currentCell.y += step.y;

							if(out.y == currentCell.y)
							{
								break;
							}
							nextCrossing.y += deltaT.y;

						}
						if(stepAxis == 2)
						{
							currentCell.z += step.z;
							
							if(out.z == currentCell.z)
							{
								break;
							}
							nextCrossing.z += deltaT.z;

						}

					}
				}
					/*
					// gridIntersect = minT * ray.dir + origin
					if(ray.direction.x > 0)
					{
						nextCrossing.x = minVal + (voxelToPosition(box, currentCell + 1,voxelWidth).x- (minVal * ray.direction + ray.origin).x) / ray.direction.x;
						deltaT.x = voxelWidth.x/ ray.direction.x;
						step.x = 1;
						out.x = numberOfVoxels.x;

					}
					else
					{
						nextCrossing.x = minVal + voxelToPosition(box, currentCell,voxelWidth).x- (minVal * ray.direction + ray.origin).x) / ray.direction.x;
						deltaT.x= -voxelWidth.x/ ray.direction.x;
						step.x = -1;
						out.x = -1;
					}


					if(ray.direction.y > 0)
					{
						nextCrossing.y = minVal + (voxelToPosition(box, currentCell + 1,voxelWidth).y - (minVal * ray.direction + ray.origin).y) / ray.direction.y;
						deltaT.y = voxelWidth.y/ ray.direction.y;
						step.y = 1;
						out.y = numberOfVoxels.y;

					}
					else
					{
						nextCrossing.y = minVal + voxelToPosition(box, currentCell,voxelWidth).y- (minVal * ray.direction +ray.origin).y) / ray.direction.y;
						deltaT.y= -voxelWidth.y/ ray.direction.y;
						step.y = -1;
						out.y= -1;
					}

					if(ray.direction.z > 0)
					{
						nextCrossing.z = minVal + (voxelToPosition(box, currentCell + 1,voxelWidth).z- (minVal * ray.direction + ray.origin).z) / ray.direction.z;
						deltaT.z = voxelWidth.z/ ray.direction.z;
						step.z = 1;
						out.z = numberOfVoxels.z;

					}
					else
					{
						nextCrossing.z = minVal + voxelToPosition(box, currentCell,voxelWidth).z - (minVal * ray.direction + ray.origin).z) / ray.direction.z;
						deltaT.z= -voxelWidth.z/ ray.direction.z;
						step.z = -1;
						ot.z = -1;
					}

					int bits =0;
					float lastDistance = 10000000.0f;
					while(true)
					{
						int cellObjectNumber = cellIndices[cellIndex]- cellIndices[cellIndex-1];

						for(int i = 0; i <  cellObjectNumber; i++ )
						{
							int objectIndex = objectIndices[cellIndices[cellIndex-1] + i]-1;

							if(objects[objectIndex].position.w == 0.0)
							{
								TriangleInfo triInfo = triangleCollision(ray,triangles[objects[objectIndex].triangleIndex]);
								if(triInfo.hasIntersection)
								{
									outColor = (uint4)adsLightT(objects[objectIndex], light[0], triInfo);
									lastDistance = triInfo.distanceFromIntersection;									
									//run = false;
									break;
								}

							}
							else
							{
								SphereInfo sphereInfo = sphereIntersection(ray,objects[objectIndex].position.xyz,objects[objectIndex].position.w);
								if(sphereInfo.hasIntersection && sphereInfo.distanceToIntersection < lastDistance)
								{
									outColor = (uint4)adsLightS(objects[objectIndex], light[0], sphereInfo);
									lastDistance = sphereInfo.distanceToIntersection;									
									//run = false;
									break;
								}
							}
						}

						bits = ((nextCrossing.x < nextCrossing.y) << 2) + 
						(nextCrossing.x < nextCrossing.z) << 2) + 
						(nextCrossing.y < nextCrossing.z) << 2));

						int stepAxis = cmpAxis[bits];

						if(maxVal < nextCrossing[stepAxis])
						{
							break;
						}
						currentCell[stepAxis] += step[stepAxis];
						if(currentCell[stepAxis] == out[stepAxis])
						{
							break;
						}
						nextCrossing[stepAxis] += deltaT[stepAxis];
					}


					//float3 delta = (box.max - box.min)/cellDimensions ;
					//float3 next = (convert_float3(currentCell) + 1.0f) * delta;
					//bool run =  true;
					//float lastDistance = 100000000.0f;
					



					while(run)
					{
						int cellObjectNumber = cellIndices[cellIndex]- cellIndices[cellIndex-1];

						for(int i = 0; i <  cellObjectNumber; i++ )
						{
							int objectIndex = objectIndices[cellIndices[cellIndex-1] + i]-1;

							if(objects[objectIndex].position.w == 0.0)
							{
								TriangleInfo triInfo = triangleCollision(ray,triangles[objects[objectIndex].triangleIndex]);
								if(triInfo.hasIntersection)
								{
									outColor = (uint4)adsLightT(objects[objectIndex], light[0], triInfo);
									lastDistance = triInfo.distanceFromIntersection;
					outColor = (uint4)(0,200,0,255);
									
									run = false;
								}

							}
							else
							{
								SphereInfo sphereInfo = sphereIntersection(ray,objects[objectIndex].position.xyz,objects[objectIndex].position.w);
								if(sphereInfo.hasIntersection && sphereInfo.distanceToIntersection < lastDistance)
								{
									outColor = (uint4)adsLightS(objects[objectIndex], light[0], sphereInfo);
									lastDistance = sphereInfo.distanceToIntersection;
					outColor = (uint4)(0,200,0,255);
									
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

					*/
				

		//	}
		//}
	//outColor /=samples;
	write_imageui(dstImage, outImageCoord, outColor);

}

