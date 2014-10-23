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
						//currentCell = positionToVoxel(p, voxelInvWidth, numberOfVoxels-1,box);
					    currentCell = (int3)(convert_int3(clamp((p - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), cellDimensions - 1.0f)));


					}
					else
					{
						//currentCell = positionToVoxel(ray.origin, voxelInvWidth, numberOfVoxels-1,box);
						currentCell = (int3)(convert_int3(clamp((ray.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));
						maxVal = hitCheck.maxValue;

					}					

					int cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;


					float3 nextCrossing;
					float3 deltaT;
					int3 nextStep;
					int3 out;
					float intersection;

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

					
					float lastDistance = 10000000.0f;
					bool run = true;

					while(run)
					{
						
						int cellObjectNumber = (cellIndex > 0) ? cellIndices[cellIndex]- cellIndices[cellIndex-1]  : cellIndices[0];
						//if(cellIndex < numberOfVoxels.x * numberOfVoxels.y * numberOfVoxels.z)
						//{
							for(int i = 0; i <  cellObjectNumber; i++ )
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
											run = false;
											//break;
										}
									}
									else
									{
										SphereInfo sphereInfo = sphereIntersection(ray,objects[objectIndex].position.xyz,objects[objectIndex].position.w);
										if(sphereInfo.hasIntersection && sphereInfo.distanceToIntersection < lastDistance)
										{
											outColor = (uint4)adsLightS(objects[objectIndex], light[0], sphereInfo);
											lastDistance = sphereInfo.distanceToIntersection;									
											run = false;
											//break;
										}
									}
								
							}
						//}
						int stepAxis =  (nextCrossing.x  < nextCrossing.y && nextCrossing.x < nextCrossing.z) ? 0 :(nextCrossing.y < nextCrossing.z) ? 1 : 2;



						if(stepAxis == 0)
						{


								if(maxVal < nextCrossing.x) 
								{
									run = false;
								}
								currentCell.x += nextStep.x;

						

								//if((ray.direction.x >= 0) ?  out.x <= currentCell.x : out.x >= currentCell.x)
								if( out.x == currentCell.x)
								{
									run = false;

								}
							
								nextCrossing.x += deltaT.x;

							
			
						}


						if(stepAxis == 1)
						{

								if(maxVal < nextCrossing.y)
								{
									run = false;
								}

								currentCell.y += nextStep.y;
								//if((ray.direction.y >= 0) ?  out.y <= currentCell.y : out.y >= currentCell.y)
								if( out.y == currentCell.y)
								{
									run = false;
								}
								nextCrossing.y += deltaT.y;

						}


						if(stepAxis == 2)
						{

								if(maxVal < nextCrossing.z)
								{
									run = false;
								}

								currentCell.z += nextStep.z;
								
								//if(out.z >= currentCell.z)
								if(out.z == currentCell.z)
								{
									run = false;
								}
								nextCrossing.z += deltaT.z;
							
						}

						cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
					}
	}
	write_imageui(dstImage, outImageCoord, outColor);

}

