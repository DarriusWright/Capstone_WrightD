unsigned int part(unsigned int n)
{
	
	n = (n ^ (n<<16))& 0xff0000ff;
	n = (n ^ (n<<8)) & 0x0300f00f;
	n = (n ^ (n<<4)) & 0x030c30c3;
	n = (n ^ (n<<2)) & 0x09249249;

	return n;
}





unsigned int mCode (unsigned int x,unsigned int y,unsigned int z)
{
	return (part(z) << 2) +(part(y) << 1) + part(x);
}

unsigned int mCode(uint3 position)
{
	return mCode(position.x, position.y,position.z);
}



__kernel void initializeCells(__global Object * objects)
{
	int objectIndex = get_global_id(0);

	float3 position = objects[objectIndex].box.min + ((objects[objectIndex].box.max -objects[objectIndex].box.min) /2.0f);

	objects[objectIndex].code = mCode(convert_uint3(position));
}