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


int main()
{

	BBox b(glm::vec3(10,-10,70), glm::vec3(40,20,100));

	cout << mortonCode(b) << endl << endl;

	system("pause");
	return 0;
}