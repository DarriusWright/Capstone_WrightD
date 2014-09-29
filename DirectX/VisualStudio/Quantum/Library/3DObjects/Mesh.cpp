#include "Mesh.h"
#include <3DObjects\Model.h>
#include <Window\Game.h>
#include <Exceptions\GameException.h>
#include <DataManagement\VertexManager.h>
#include <assimp\scene.h>
namespace Library
{
	Mesh::Mesh(Model & model, ModelMaterial * material) : mModel(model), mMaterial(material), mVertices(), mTextureCoords(), mTangents(), mNormals(), mBiTangents(), mVertexColors(), mFaceCount(0),
		mIndices(),
		mWorldMatrix(XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f))
	{

	}


	void Mesh::left()
	{
		mWorldMatrix._42 = 2;

		mWorldMatrix._43 = -5;
	}

	void Mesh::right()
	{
		mWorldMatrix._42 = -2;

		mWorldMatrix._43 = -5;
	}

	Mesh::Mesh(Model & model, aiMesh& mesh) : mModel(model), mMaterial(nullptr), mName(mesh.mName.C_Str())
		, mVertices(), mTextureCoords(), mTangents(), mNormals(), mBiTangents(), mVertexColors(), mFaceCount(0),
		mIndices(), mWorldMatrix(XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f))
	{
		mMaterial = mModel.materials().at(mesh.mMaterialIndex);
		mVertices.reserve(mesh.mNumVertices);
		for (UINT i = 0; i < mesh.mNumVertices; i++)
		{
			mVertices.push_back(XMFLOAT3(reinterpret_cast<const float*>(&mesh.mVertices[i])));
		}

		if (mesh.HasNormals())
		{
			mNormals.reserve(mesh.mNumVertices);
			for (UINT i = 0; i < mesh.mNumVertices; i++)
			{
				mNormals.push_back(XMFLOAT3(reinterpret_cast<const float*>(&mesh.mNormals[i])));
			}
		}

		if (mesh.HasTangentsAndBitangents())
		{
			mTangents.reserve(mesh.mNumVertices);
			mBiTangents.reserve(mesh.mNumVertices);
			for (UINT i = 0; i < mesh.mNumVertices; i++)
			{
				mTangents.push_back(XMFLOAT3(reinterpret_cast<const float*>(&mesh.mTangents[i])));
				mBiTangents.push_back(XMFLOAT3(reinterpret_cast<const float*>(&mesh.mBitangents[i])));
			}
		}

		UINT uvChannelCount = mesh.GetNumUVChannels();

		for (UINT i = 0; i < uvChannelCount; i++)
		{
			std::vector<XMFLOAT3>* textureCoordinates = new std::vector<XMFLOAT3>();
			textureCoordinates->reserve(mesh.mNumVertices);
			mTextureCoords.push_back(textureCoordinates);

			aiVector3D * aiTextureCoordinates = mesh.mTextureCoords[i];
			for (UINT j = 0; j < mesh.mNumVertices; j++)
			{
				textureCoordinates->push_back(XMFLOAT3(reinterpret_cast<const float*>(&aiTextureCoordinates[j])));
			}
		}


		UINT colorChannelCount = mesh.GetNumColorChannels();

		for (UINT i = 0; i < colorChannelCount; i++)
		{
			std::vector<XMFLOAT4>* vertexColors = new std::vector<XMFLOAT4>();
			vertexColors->reserve(mesh.mNumVertices);
			mVertexColors.push_back(vertexColors);

			aiColor4D * aiVertexColors = mesh.mColors[i];
			for (UINT j = 0; j < mesh.mNumVertices; j++)
			{
				vertexColors->push_back(XMFLOAT4(reinterpret_cast<const float*>(&aiVertexColors[j])));
			}
		}

		if (mesh.HasFaces())
		{
			mFaceCount = mesh.mNumFaces;
			for (UINT i = 0; i < mFaceCount; i++)
			{
				aiFace * face = &mesh.mFaces[i];
				for (UINT j = 0; j < face->mNumIndices; j++)
				{
					mIndices.push_back(face->mIndices[j]);
				}
			}
		}

	}
	Mesh::~Mesh()
	{
		for (std::vector<XMFLOAT3>* textureCoordinates : mTextureCoords)
		{
			delete textureCoordinates;
		}

		for (std::vector<XMFLOAT4>*vertexColor : mVertexColors)
		{
			delete vertexColor;
		}
		
	}
	XMFLOAT4X4 & Mesh::getWorldMatrix()
	{ 
		return mWorldMatrix;
	}

	Model& Mesh::getModel(){ return mModel; }
	ModelMaterial * Mesh::getMaterial(){ return mMaterial; }
	const std::string & Mesh::name()const{ return mName; }
	const std::vector<XMFLOAT3>& Mesh::vertices()const{ return mVertices; }
	const std::vector<XMFLOAT3>& Mesh::normals()const{ return mNormals; }
	const std::vector<XMFLOAT3>& Mesh::tangents()const{ return mTangents; }
	const std::vector<XMFLOAT3>& Mesh::biTangents()const{ return mBiTangents; }
	const std::vector<std::vector<XMFLOAT3>*>& Mesh::textureCoords()const{ return mTextureCoords; }
	const std::vector<std::vector<XMFLOAT4>*>& Mesh::vertexColors()const{ return mVertexColors; }

	UINT Mesh::faceCount()const{ return mFaceCount; }
	const std::vector<UINT>& Mesh::indices()const{ return mIndices; }
	void Mesh::createIndexBuffer(ID3D11Buffer ** indexBuffer)
	{
		assert(indexBuffer != nullptr);

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc,sizeof(indexBufferDesc));
		indexBufferDesc.ByteWidth = sizeof(UINT)* mIndices.size();
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA indexSubResourceData;
		ZeroMemory(&indexSubResourceData, sizeof(indexSubResourceData));
		indexSubResourceData.pSysMem = &mIndices[0];
		if (FAILED(mModel.getGame().device()->CreateBuffer(&indexBufferDesc, &indexSubResourceData, indexBuffer)))
		{
			throw  GameException("ID3D11Device::CreateBuffer() failed");
		}
	}


	ID3D11Buffer * Mesh::getVertexBuffer()
	{
		return mVertexBuffer;
	}
	ID3D11Buffer * Mesh::getIndexBuffer()
	{
		return mIndexBuffer;
	}

	void Mesh::setVertexBuffer(ID3D11Buffer * buffer)
	{
		mVertexBuffer = buffer;
	}
	void Mesh::setIndexBuffer(ID3D11Buffer * buffer)
	{
		mIndexBuffer = buffer;
	}


	UINT & Mesh::getVertexOffset()
	{
		return mVertexOffset;
	}
	void Mesh::setVertexOffset(UINT offset)
	{
		mVertexOffset = offset;
	}

	UINT & Mesh::getIndexOffset()
	{
		return mIndexOffset;
	}
	void Mesh::setIndexOffset(UINT offset)
	{
		mIndexOffset = offset;
	}
}