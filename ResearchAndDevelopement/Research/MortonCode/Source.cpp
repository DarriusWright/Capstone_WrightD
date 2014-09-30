#include <iostream>
#include <glm.hpp>

using namespace std;


struct BBox
{
	BBox(glm::vec3 min,
	glm::vec3 max) : min(min),max(max)
	{

	}

	glm::vec3 min;
	glm::vec3 max;
};


inline uint64_t mortonEncode_for(unsigned int x, unsigned int y, unsigned int z){
    uint64_t answer = 0;
	int number = (sizeof(uint64_t)* CHAR_BIT)/3;
    for (uint64_t i = 0; i < (sizeof(uint64_t)* CHAR_BIT)/3; ++i) {
        answer |= ((x & ((uint64_t)1 << i)) << 2*i) | ((y & ((uint64_t)1 << i)) << (2*i + 1)) | ((z & ((uint64_t)1 << i)) << (2*i + 2));
    }
    return answer;
}




unsigned int mortonCode(BBox b)
{
	glm::vec3 bbMin = 3.0f* b.min;
	glm::vec3 bbMax = 3.0f* b.max;

	glm::vec3 invBox = 1024.0f / (bbMin - bbMax);


	glm::vec3 center = b.min + ((b.max - b.min)/2.0f);
	glm::vec3 primComp = ((center - bbMin) * invBox);

	unsigned int zComp = ((int)primComp.z  & 1);
	unsigned int yComp = ((int)primComp.y  & 1) << 1;
	unsigned int xComp = ((int)primComp.x  & 1) << 2;


	unsigned int mCode = zComp | yComp | xComp;

	int shift3 = 2;
	int shift = 2;

	for (int i = 1; i < 10; i++)
	{
		mCode |= ((unsigned int)center.z & shift) << shift3++;
		mCode |= ((unsigned int)center.y & shift) << shift3++;
		mCode |= ((unsigned int)center.x & shift) << shift3++;

		shift <<= 1;
	}


	return mCode;

}

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





int main()
{

	BBox b(glm::vec3(10,-10,70), glm::vec3(40,20,100));

	cout << mortonCode(b) << endl << endl;
	glm::vec3 centerPoint = b.min + ((b.max - b.min)/2.0f);
	cout << mortonEncode_for(centerPoint.x ,centerPoint.y,centerPoint.z)  << endl;
	cout << mCode(4,2,1)<< endl;
	system("pause");
	return 0;
}