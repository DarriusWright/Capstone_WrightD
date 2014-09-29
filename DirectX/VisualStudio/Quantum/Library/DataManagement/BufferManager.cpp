#include "BufferManager.h"
#include <Exceptions\GameException.h>
#include <Window\Game.h>


namespace Library

{

	BufferManager::BufferManager(Game & game, D3D11_BIND_FLAG bindFlag) : mGame(&game), buffer(nullptr)
		, mBindFlag(bindFlag)
	{
	}


	BufferManager::~BufferManager()
	{
		ReleaseObject(buffer);
	}

	void BufferManager::initialize()
	{
		byte * mData = new byte[MEGABYTE];
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.ByteWidth = MEGABYTE;
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.BindFlags = mBindFlag;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexSubResourceData;
		ZeroMemory(&vertexSubResourceData, sizeof(vertexSubResourceData));
		vertexSubResourceData.pSysMem = &mData[0];
		vertexSubResourceData.SysMemPitch = 0;
		vertexSubResourceData.SysMemSlicePitch = 0;

		if (FAILED(mGame->device()->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, &buffer)))
		{
			throw GameException(L"ID3D11Device::CreateBuffer() failed");
		}

		DeleteObjects(mData);
	}

	ID3D11Buffer * BufferManager::getBuffer() const
	{
		return buffer;
	}
	void BufferManager::setBuffer(ID3D11Buffer * buffer)
	{
		this->buffer = buffer;
	}

}