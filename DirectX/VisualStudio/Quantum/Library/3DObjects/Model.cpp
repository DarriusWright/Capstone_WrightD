#include "Model.h"
#include <Exceptions\GameException.h>
#include <3DObjects\Mesh.h>
#include <3DObjects\ModelMaterial.h>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <DataManagement\MeshManager.h>
#include <Window\Game.h>
#include <WICTextureLoader.h>


namespace Library
{

	Model::Model(Game & game, const std::string & fileName,ID3DX11Effect * effect, const wchar_t * texturePath, bool flipUvs) : mGame(game), mMeshes(), mMaterials()
	{
		Assimp::Importer importer;
		UINT flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_FlipWindingOrder;
		if (flipUvs)
		{
			flags |= aiProcess_FlipUVs;
		}

		const aiScene * scene = importer.ReadFile(fileName, flags);
		if (scene == nullptr)
		{
			throw GameException(importer.GetErrorString());
		}

		if (scene->HasMaterials())
		{
			for (UINT i = 0; i < scene->mNumMaterials; i++)
			{
				mMaterials.push_back(new ModelMaterial(*this, scene->mMaterials[i]));
			}
		}
		if (scene->HasMeshes())
		{
			for (UINT i = 0; i < scene->mNumMeshes; i++)
			{
				Mesh * mesh = new Mesh(*this, *(scene->mMeshes[i]));
				mMeshes.push_back(mesh);
				//mGame.meshManager()->addMesh(mesh);
			}
		}


		ID3DX11EffectVariable * variable = effect->GetVariableByName("ColorTexture");

		if (variable == nullptr)
		{
			throw GameException(L"ID3DX11Effect::GetVariableByName() unable to find variable ColorTexture");
		}

		mColorTextureVarialble = variable->AsShaderResource();
		if (!mColorTextureVarialble->IsValid())
		{
			throw GameException("Invalid effect variable cast.");
		}


		std::wstring textureName = texturePath;
		HRESULT hr;
		if (FAILED(hr = DirectX::CreateWICTextureFromFile(mGame.device(), mGame.deviceContext(), textureName.c_str(), nullptr, &mTextureShaderResourceView)))
		{
			throw GameException(L"CreateWICTextureFromFile failed", hr);
		}

	}

	Model::Model(Game & game, const std::string & fileName, bool flipUvs) : mGame(game), mMeshes(), mMaterials()
	{
		Assimp::Importer importer;
		UINT flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_FlipWindingOrder;
		if (flipUvs)
		{
			flags |= aiProcess_FlipUVs;
		}

		const aiScene * scene = importer.ReadFile(fileName, flags);
		if (scene == nullptr)
		{
			throw GameException(importer.GetErrorString());
		}

		if (scene->HasMaterials())
		{
			for (UINT i = 0; i < scene->mNumMaterials; i++)
			{
				mMaterials.push_back(new ModelMaterial(*this, scene->mMaterials[i]));
			}
		}
		if (scene->HasMeshes())
		{
			for (UINT i = 0; i < scene->mNumMeshes; i++)
			{
				Mesh * mesh = new Mesh(*this, *(scene->mMeshes[i]));
				mMeshes.push_back(mesh);
				//mGame.meshManager()->addMesh(mesh);
			}
		}

	}


	ID3D11ShaderResourceView * Model::textureShaderResourceView()
	{
		return mTextureShaderResourceView;
	}
	ID3DX11EffectShaderResourceVariable * Model::colorTextureVarialble()
	{
		return mColorTextureVarialble;
	}

	Model::~Model()
	{
		for (Mesh * m : mMeshes)
		{
			delete m;
		}

		for (ModelMaterial * material : mMaterials)
		{
			delete material;
		}
	}

	void  Model::setPosition(const XMFLOAT3 & position)
	{
		mPosition = position;
	}
	void  Model::setScale(const XMFLOAT3 & scale)
	{
		mScale = scale;
	}
	void  Model::setRotation(const XMFLOAT3 & rotation)
	{
		mRotation = rotation;
	}

	const XMFLOAT3 & Model::position()
	{
		return mPosition;
	}
	const XMFLOAT3 & Model::scale()
	{
		return mScale;
	}
	const XMFLOAT3 & Model::rotation()
	{
		return mRotation;
	}

	Game& Model::getGame()
	{
		return mGame;
	}
	bool Model::hasMeshes()const
	{
		return mMeshes.size() > 0;
	}
	bool Model::hasMaterials()const
	{
		return mMaterials.size() > 0;
	}

	const std::vector<Mesh*>& Model::meshes()const
	{
		return mMeshes;
	}
	const std::vector<ModelMaterial*>& Model::materials()const
	{
		return mMaterials;
	}

}