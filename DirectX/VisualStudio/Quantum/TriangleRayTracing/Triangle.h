#pragma once
#include <Misc\Common.h>

struct Triangle
{
public:
	Triangle();
	~Triangle();

	XMFLOAT3 v0;
	XMFLOAT3 v1;
	XMFLOAT3 v2;
	XMFLOAT3 normal;
};

