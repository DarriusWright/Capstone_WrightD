#include "FullScreenQuad.h"
#include <Window\Game.h>
#include <Exceptions\GameException.h>
#include <Materials\Material.h>
#include <Effects\Effect.h>
#include <Effects\Pass.h>

namespace Library
{
	RTTI_DEFINITIONS(FullScreenQuad)

		FullScreenQuad::FullScreenQuad(Game& game)
		: DrawableGameComponent(game),
		mMaterial(nullptr), mPass(nullptr), mInputLayout(nullptr),
		mVertexBuffer(nullptr), mIndexBuffer(nullptr), mVertexCount(0), mIndexCount(0), mCustomUpdateMaterial(nullptr)
	{
		}

	FullScreenQuad::FullScreenQuad(Game& game, Material& material)
		: DrawableGameComponent(game),
		mMaterial(&material), mPass(nullptr), mInputLayout(nullptr),
		mVertexBuffer(nullptr), mIndexBuffer(nullptr), mVertexCount(0), mIndexCount(0), mCustomUpdateMaterial(nullptr)
	{
	}

	FullScreenQuad::~FullScreenQuad()
	{
		ReleaseObject(mIndexBuffer);
		ReleaseObject(mVertexBuffer);
	}

	Material* FullScreenQuad::GetMaterial()
	{
		return mMaterial;
	}

	void FullScreenQuad::SetMaterial(Material& material, const std::string& techniqueName, const std::string& passName)
	{
		mMaterial = &material;
		SetActiveTechnique(techniqueName, passName);
	}

	void FullScreenQuad::SetActiveTechnique(const std::string& techniqueName, const std::string& passName)
	{
		Technique* technique = mMaterial->getEffect()->techniquesByName().at(techniqueName);
		assert(technique != nullptr);

		mPass = technique->passesByName().at(passName);
		assert(mPass != nullptr);
		mInputLayout = mMaterial->inputLayouts().at(mPass);
	}

	void FullScreenQuad::SetCustomUpdateMaterial(std::function<void()> callback)
	{
		mCustomUpdateMaterial = callback;
	}

	void FullScreenQuad::initialize()
	{
		VertexPositionTexture vertices[] =
		{
			VertexPositionTexture(XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)),
			VertexPositionTexture(XMFLOAT4(-1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)),
			VertexPositionTexture(XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)),
			VertexPositionTexture(XMFLOAT4(1.0f, -1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)),
		};

		mVertexCount = ARRAYSIZE(vertices);

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.ByteWidth = sizeof(VertexPositionTexture)* mVertexCount;
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexSubResourceData;
		ZeroMemory(&vertexSubResourceData, sizeof(vertexSubResourceData));
		vertexSubResourceData.pSysMem = vertices;
		if (FAILED(mGame->device()->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, &mVertexBuffer)))
		{
			throw GameException("ID3D11Device::CreateBuffer() failed.");
		}

		UINT indices[] =
		{
			0, 1, 2,
			0, 2, 3
		};

		mIndexCount = ARRAYSIZE(indices);

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.ByteWidth = sizeof(UINT)* mIndexCount;
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexSubResourceData;
		ZeroMemory(&indexSubResourceData, sizeof(indexSubResourceData));
		indexSubResourceData.pSysMem = indices;
		if (FAILED(mGame->device()->CreateBuffer(&indexBufferDesc, &indexSubResourceData, &mIndexBuffer)))
		{
			throw GameException("ID3D11Device::CreateBuffer() failed.");
		}
	}

	void FullScreenQuad::draw(const GameTimer& gameTime)
	{
		assert(mPass != nullptr);
		assert(mInputLayout != nullptr);

		ID3D11DeviceContext* direct3DDeviceContext = mGame->deviceContext();
		direct3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		direct3DDeviceContext->IASetInputLayout(mInputLayout);

		UINT stride = sizeof(VertexPositionTexture);
		UINT offset = 0;
		direct3DDeviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
		direct3DDeviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		if (mCustomUpdateMaterial != nullptr)
		{
			mCustomUpdateMaterial();
		}

		mPass->apply(0, direct3DDeviceContext);

		direct3DDeviceContext->DrawIndexed(mIndexCount, 0, 0);
	}
}