__kernel void findObjectCells(__global Object * objects,__global BBox * box,
	__global int * cells,
 __global int * cellIndices,__global int * objectIndices ,
	 __global int * cellIndexIncrement , float3 delta, float3 deltaInv, float3 widthInverse , float3 numberOfVoxels)
{
	int objectIndex = get_global_id(0);	

	BBox oBox = objects[objectIndex].box;

	int3 cellMin = positionToVoxel(oBox.min + objects[objectIndex].position,widthInverse, numberOfVoxels,box[0] );
	int3 cellMax = positionToVoxel(oBox.max+ objects[objectIndex].position,widthInverse, numberOfVoxels,box[0] );

	for(int z = cellMin.z; z < cellMax.z; z++)
	{
		for(int y = cellMin.y; y < cellMax.y; y++)
		{
			for(int x = cellMin.x; x < cellMax.x; x++)
			{
				int cellsIndex = findVoxelIndex((int3)(x,y,z) , numberOfVoxels);
				int currentIndex = (cellsIndex != 0) ?  cellIndices[cellsIndex-1] : 0;
				int offsetIndex = currentIndex + (atom_inc(&cellIndexIncrement[cellsIndex]));
				objectIndices[offsetIndex] = objectIndex + 1;
			}
		}

	}
}

/*
__kernel void findObjectCells(__global Object * objects,__global BBox * box,
	__global int * cells,
 __global int * cellIndices,__global int * objectIndices ,
	__global BBox * cBox, __global int * cellIndexIncrement)
{
	int objectIndex = get_global_id(0);	
	int cellsIndex = get_global_id(1);	

	if(bboxObjectCollided(cBox[cellsIndex],objects[objectIndex]) )
	{
		int currentIndex = (cellsIndex == 0) ?  0 : cellIndices[cellsIndex-1];
		bool found = false;

		int offsetIndex = currentIndex + (atom_inc(&cellIndexIncrement[cellsIndex]));

		objectIndices[offsetIndex] = objectIndex + 1;	
			
		
		
	}
}
*/