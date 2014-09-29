#include "TriMesh.h"
#include <Exceptions\GameException.h>
#include <3DObjects\Mesh.h>
#include <3DObjects\ModelMaterial.h>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <DataManagement\MeshManager.h>
#include <Window\Game.h>
#include <WICTextureLoader.h>



TriMesh::TriMesh(Game & game, const std::string & fileName) : mGame(&game)
{
	Assimp::Importer importer;
	UINT flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType |aiProcess_FlipWindingOrder;
	//if (flipUvs)
	//{
	//	flags |= aiProcess_FlipUVs;
	//}
		const aiScene * scene = importer.ReadFile(fileName, flags);
		aiMesh * mesh = (scene->mMeshes[0]);
		if (mesh->HasFaces())
		{
			int mFaceCount = mesh->mNumFaces;
			for (UINT i = 0; i < mFaceCount; i++)
			{
				aiFace * face = &mesh->mFaces[i];
				for (UINT j = 0; j < face->mNumIndices; j++)
				{
					mIndices.push_back(face->mIndices[j]);
				}
			}
		}

		mVertices.reserve(mesh->mNumVertices);
		for (size_t i = 0; i < mesh->mNumVertices; i++)
		{
			mVertices.push_back(XMFLOAT3(reinterpret_cast<const float*>(&mesh->mVertices[i])));
		}

		mTriangles.reserve(mIndices.size() / 3);
		for (UINT i = 0; i < mIndices.size(); i+=3)
		{
			Triangle tri;
			tri.v0 = XMFLOAT3(reinterpret_cast<const float*>(&mesh->mVertices[mIndices[i]]));
			tri.v1 = XMFLOAT3(reinterpret_cast<const float*>(&mesh->mVertices[mIndices[i + 1]]));
			tri.v2 = XMFLOAT3(reinterpret_cast<const float*>(&mesh->mVertices[mIndices[i + 2]]));
			if (mesh->HasNormals())
				tri.normal = XMFLOAT3(reinterpret_cast<const float*>(&mesh->mNormals[mIndices[i]]));

			mTriangles.push_back(tri);
		}




		//if (mesh->HasNormals()){}
		/*{
			mNormals.reserve(mesh.mNumVertices);
			for (UINT i = 0; i < mesh.mNumVertices; i++)
			{
				mNormals.push_back(XMFLOAT3(reinterpret_cast<const float*>(&mesh.mNormals[i])));
			}
		}*/


}


TriMesh::~TriMesh()
{
}
