#include "VertexManager.h"
#include <3DObjects\Mesh.h>
#include <Misc\Colors.h>
#include <Exceptions\GameException.h>
#include <Window\Game.h>
namespace Library
{

	VertexManager::VertexManager(Game & game) : BufferManager(game, D3D11_BIND_VERTEX_BUFFER), mVertexCount(0)//, mVertices()
	{
	}

	VertexManager::~VertexManager()
	{

	}
	//template<class T>
	void VertexManager::addMesh(Mesh * mesh)
	{
		if (mesh->vertices().size() < spaceLeft())
		{
			UINT offset = mVertexCount * sizeof(TextureMappingVertex);
			mesh->setVertexOffset(offset);
			mesh->setVertexBuffer(buffer);
			if (mesh->textureCoords().size() > 0)
			{


				ID3D11DeviceContext* context = mGame->deviceContext();
				D3D11_MAPPED_SUBRESOURCE resource;
				HRESULT hr = context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
				TextureMappingVertex * vertices = (TextureMappingVertex*)resource.pData;

				std::vector<XMFLOAT3>* vertexColors = mesh->textureCoords().at(0);

				for (UINT i = 0; i < mesh->vertices().size(); i++)
				{
					XMFLOAT4 position = { mesh->vertices()[i].x, mesh->vertices()[i].y, mesh->vertices()[i].z, 1.0f };
					vertices[mVertexCount].mPosition = position;
					vertices[mVertexCount].mTextureCoordinates = { vertexColors->at(i).x, vertexColors->at(i).y };
					mVertexCount++;
				}

				context->Unmap(buffer, 0);

			}
			//else
			//{
			//	for (UINT i = 0; i < mesh->vertices().size(); i++)
			//	{
			//		XMFLOAT4 position = { mesh->vertices()[i].x , mesh->vertices()[i].y , mesh->vertices()[i].z,1.0f};
			//		XMFLOAT4 color = { 0.2f, 0.3f, 0.8f, 1.0f };
			//		vertices[mVertexCount].mPosition = position;
			//		vertices[mVertexCount].mColor = color;
			//		mVertexCount++;
			//	}
			//}



		}
	}
	UINT VertexManager::spaceLeft()
	{
		return MAX_VERTICES - mVertexCount;
	}

	void VertexManager::initialize()
	{
		//mVertices.resize(MAX_VERTICES);
		BufferManager::initialize();
	}

}

//#include "VertexManager.h"
//#include <3DObjects\Mesh.h>
//#include <Misc\Colors.h>
//#include <Exceptions\GameException.h>
//#include <Window\Game.h>
//namespace Library
//{
//
//	VertexManager::VertexManager(Game & game) : BufferManager(game), mVertexCount(0), mVertices()
//	{
//	}
//
//	VertexManager::~VertexManager()
//	{
//	}
//
//	void VertexManager::addMesh(Mesh * mesh)
//	{
//		if (mesh->vertices().size() < spaceLeft())
//		{
//			UINT offset = mVertexCount * sizeof(TextureMappingVertex);
//			mesh->setVertexOffset(offset);
//			mesh->setVertexBuffer(buffer);
//			mesh->setVertexManager(this);
//
//			if (mesh->vertexColors().size() > 0)
//			{
//				std::vector<XMFLOAT4>* vertexColors = mesh->vertexColors().at(0);
//
//				for (UINT i = 0; i < mesh->vertices().size(); i++)
//				{
//
//					mVertices[mVertexCount].mPosition.x = mesh->vertices()[i].x;
//					mVertices[mVertexCount].mPosition.y = mesh->vertices()[i].y;
//					mVertices[mVertexCount].mPosition.z = mesh->vertices()[i].z;
//					mVertices[mVertexCount].mPosition.w = 1.0f;
//
//					mVertices[mVertexCount].mColor = vertexColors->at(i);
//					mVertexCount++;
//				}
//
//			}
//			else
//			{
//
//
//				ID3D11DeviceContext* context = mGame->deviceContext();
//
//				D3D11_MAPPED_SUBRESOURCE resource;
//				HRESULT hr = context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
//				TextureMappingVertex * vertices = (TextureMappingVertex*)resource.pData;
//
//				for (UINT i = 0; i < mesh->vertices().size(); i++)
//				{
//					mVertices[mVertexCount].mPosition.x = mesh->vertices()[i].x;
//					mVertices[mVertexCount].mPosition.y = mesh->vertices()[i].y;
//					mVertices[mVertexCount].mPosition.z = mesh->vertices()[i].z;
//					mVertices[mVertexCount].mPosition.w = 1.0f;
//
//					mVertices[mVertexCount].mColor.x = 0.2f;
//					mVertices[mVertexCount].mColor.y = 0.3f;
//					mVertices[mVertexCount].mColor.z = 0.8f;
//					mVertices[mVertexCount].mColor.w = 1.0f;
//
//					vertices[mVertexCount].mPosition = mVertices[mVertexCount].mPosition;
//					vertices[mVertexCount].mColor = mVertices[mVertexCount].mColor;
//					mVertexCount++;
//				}
//
//				context->Unmap(buffer, 0);
//				context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
//				vertices = (TextureMappingVertex*)resource.pData;
//				for (size_t i = 0; i < mVertexCount; i++)
//				{
//					//TextureMappingVertex vertex = { vertices[i].mPosition,vertices[i].mColor };
//					XMFLOAT4 position = { vertices[i].mPosition.x, vertices[i].mPosition.y, vertices[i].mPosition.z, vertices[i].mPosition.w };
//					XMFLOAT4 color = { vertices[i].mColor.x, vertices[i].mColor.y, vertices[i].mColor.z, vertices[i].mColor.w };
//				}
//
//				context->Unmap(buffer, 0);
//
//			}
//
//		}
//	}
//	UINT VertexManager::spaceLeft()
//	{
//		return MAX_VERTICES - mVertexCount;
//	}
//
//	void VertexManager::initialize()
//	{
//		mVertices.resize(MAX_VERTICES);
//		D3D11_BUFFER_DESC vertexBufferDesc;
//		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
//		vertexBufferDesc.ByteWidth = sizeof(TextureMappingVertex)* MAX_VERTICES;
//		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//		vertexBufferDesc.MiscFlags = 0;
//
//		D3D11_SUBRESOURCE_DATA vertexSubResourceData;
//		ZeroMemory(&vertexSubResourceData, sizeof(vertexSubResourceData));
//		vertexSubResourceData.pSysMem = &mVertices[0];
//		vertexSubResourceData.SysMemPitch = 0;
//		vertexSubResourceData.SysMemSlicePitch = 0;
//		
//		if (FAILED(mGame->device()->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, &buffer)))
//		{
//			throw GameException(L"ID3D11Device::CreateBuffer() failed");
//		}
//
//	}
//
//}