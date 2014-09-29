#include "Platform.h"


Platform::Platform(void)
{
	 clGetPlatformIDs(1, &platform, NULL);
}


Platform::~Platform(void)
{
}
