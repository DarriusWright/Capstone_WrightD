#pragma once

#include <Misc/Common.h>
#include <Window\Game.h>
#include"Triangle.h"

using namespace Library;
class TriMesh
{
public:
	TriMesh(Game & game, const std::string & fileName);
	~TriMesh();
	std::vector<Triangle> mTriangles;
private:
	Game * mGame;
	std::vector<XMFLOAT3> mVertices;
	std::vector<XMFLOAT3> mNormal;
	std::vector<unsigned int> mIndices;
};

