#include "IndexManager.h"
#include <3DObjects\Mesh.h>
#include <Misc\Colors.h>
#include <Exceptions\GameException.h>
#include <Vertex\BasicVertex.h>
#include <Window\Game.h>

namespace Library
{


	IndexManager::IndexManager(Game & game) : BufferManager(game, D3D11_BIND_INDEX_BUFFER), mIndexCount(0)
	{
	}


	IndexManager::~IndexManager()
	{
	}

	void IndexManager::addMesh(Mesh * mesh)
	{
		if (mesh->indices().size() < spaceLeft())
		{
			UINT sizeOfUint = sizeof(UINT);
			UINT offset = mIndexCount * sizeOfUint;
			mesh->setIndexOffset(offset);
			mesh->setIndexBuffer(buffer);


			ID3D11DeviceContext* context = mGame->deviceContext();

			D3D11_MAPPED_SUBRESOURCE resource;
			HRESULT hr = context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
			UINT * indices = (UINT*)resource.pData;

			//for (size_t i = 0; i < mIndexCount; i++)
			//{
			//	indices[i] = mIndices[i];
			//}

			for (size_t i = 0; i < mesh->indices().size(); i++)
			{
				//mIndices[mIndexCount] = mesh->indices()[i];
				indices[mIndexCount] = mesh->indices()[i];
				mIndexCount++;
			}

			context->Unmap(buffer, 0);
			
		}
	}
	UINT IndexManager::spaceLeft()
	{
		return MAX_INDICES - mIndexCount;
	}
	void IndexManager::initialize()
	{
		//mIndices.resize(MAX_INDICES);

		//D3D11_BUFFER_DESC indexBufferDesc;
		//ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		//indexBufferDesc.ByteWidth = sizeof(UINT)* MAX_INDICES;
		//indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		//indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		//indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		//indexBufferDesc.MiscFlags = 0;

		//D3D11_SUBRESOURCE_DATA indexSubResourceData;
		//ZeroMemory(&indexSubResourceData, sizeof(indexSubResourceData));
		//indexSubResourceData.pSysMem = &mIndices[0];
		//if (FAILED(mGame->device()->CreateBuffer(&indexBufferDesc, &indexSubResourceData, &buffer)))
		//{
		//	throw  GameException("ID3D11Device::CreateBuffer() failed");
		//}
		BufferManager::initialize();

	}
}