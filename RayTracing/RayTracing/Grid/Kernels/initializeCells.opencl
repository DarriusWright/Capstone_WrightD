/*
__kernel void initializeCells(__global Object * objects,__global BBox * box,__global int * cells,
	int nx, int ny ,int nz,__global BBox * cBox, __global int * totalCells
	)
{
	int objectIndex = get_global_id(0);	
	int cellsIndex = get_global_id(1);	

	BBox cellBox;
	float3 numberOfCellsVector = (float3)(nx,ny,nz);
	float3 minLocation;
	minLocation.z = cellsIndex / (nx* ny);
	minLocation.y = (cellsIndex % (nx* ny)) / nx;
	minLocation.x = cellsIndex - minLocation.y * nx - minLocation.z *nx *ny;

	float3 cellDimensions = (float3)(nx,ny,nz);
	float3 delta = (box[0].max - box[0].min)/cellDimensions ;

	cellBox.min = (float3)(box[0].min + (minLocation * delta));
	cellBox.max = (float3)(box[0].min + delta + (minLocation * delta));

	cBox[cellsIndex].min = cellBox.min;
	cBox[cellsIndex].max = cellBox.max;

	if(bboxObjectCollided(cellBox,objects[objectIndex]))
	{
		atom_inc(&cells[cellsIndex]);
		atom_inc(&totalCells[0]);
	}
	
}

*/

__kernel void initializeCells(__global Object * objects,__global BBox * box,__global int * cells,
	float3 numberOfVoxels, float3 widthInverse, __global int * totalCells)
/*{
	int objectIndex = get_global_id(0);	
	int cellsIndex = get_global_id(1);	

	BBox cellBox;
	
	float3 minLocation = convert_float3(findVoxelPosition(cellsIndex, numberOfVoxels));


	float3 cellDimensions = numberOfVoxels;
	float3 delta = (box[0].max - box[0].min)/cellDimensions ;

	cellBox.min = (float3)(box[0].min + (minLocation * delta));
	cellBox.max = (float3)(box[0].min + delta + (minLocation * delta));

	if(bboxObjectCollided(cellBox,objects[objectIndex]))
	{
		atom_inc(&cells[cellsIndex]);
		atom_inc(&totalCells[0]);
	}
	
}*/
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