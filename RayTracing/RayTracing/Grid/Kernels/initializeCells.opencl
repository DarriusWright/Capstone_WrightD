__kernel void initializeCells(__global Object * objects,__global BBox * box,__global int * cells,
	float3 numberOfVoxels, float3 widthInverse, __global int * totalCells)
{
	int objectIndex = get_global_id(0);	

	int3 cellMin = positionToVoxel(objects[objectIndex].box.min + objects[objectIndex].position.xyz,widthInverse, numberOfVoxels,box[0] );
	int3 cellMax = positionToVoxel(objects[objectIndex].box.max+ objects[objectIndex].position.xyz,widthInverse, numberOfVoxels,box[0] );
	float totalVoxels = numberOfVoxels.x * numberOfVoxels.y * numberOfVoxels.z;
	for(int z = cellMin.z; z <= cellMax.z; z++)
	{
		for(int y = cellMin.y; y <= cellMax.y; y++)
		{
			for(int x = cellMin.x; x <= cellMax.x; x++)
			{
				int index = findVoxelIndex((int3)(x,y,z) , numberOfVoxels);
				if(index < totalVoxels)
				{
					atom_inc(&cells[index]);
					atom_inc(&totalCells[0]);
				}
			}
		}
	}
}