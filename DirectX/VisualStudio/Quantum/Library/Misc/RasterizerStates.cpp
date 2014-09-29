#include "RasterizerStates.h"
#include <Exceptions\GameException.h>


//FIND MORE STATES
namespace Library
{
	
	ID3D11RasterizerState * RasterizerStates::backCulling = nullptr;
	ID3D11RasterizerState * RasterizerStates::frontCulling = nullptr;
	ID3D11RasterizerState * RasterizerStates::disableCulling = nullptr;
	ID3D11RasterizerState * RasterizerStates::wireFrame = nullptr;

	RasterizerStates::~RasterizerStates()
	{
	}

	void RasterizerStates::initialize(ID3D11Device * device)
	{
		assert(device != nullptr);

		D3D11_RASTERIZER_DESC rasterizerDesc;
		ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.DepthClipEnable = true;

		HRESULT hr = device->CreateRasterizerState(&rasterizerDesc, &backCulling);
		if (FAILED(hr))
		{
			throw GameException(L"ID3D11Device::CreateRasterizerState() failed");
		}

		rasterizerDesc;
		ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.FrontCounterClockwise = true;

		rasterizerDesc.DepthClipEnable = true;

		hr = device->CreateRasterizerState(&rasterizerDesc, &frontCulling);
		if (FAILED(hr))
		{
			throw GameException(L"ID3D11Device::CreateRasterizerState() failed");
		}

		rasterizerDesc;
		ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.DepthClipEnable = true;

		hr = device->CreateRasterizerState(&rasterizerDesc, &disableCulling);
		if (FAILED(hr))
		{
			throw GameException(L"ID3D11Device::CreateRasterizerState() failed");
		}

		rasterizerDesc;
		ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.DepthClipEnable = true;

		hr = device->CreateRasterizerState(&rasterizerDesc, &wireFrame);
		if (FAILED(hr))
		{
			throw GameException(L"ID3D11Device::CreateRasterizerState() failed");
		}


	}

	void RasterizerStates::release()
	{
		ReleaseObject(backCulling);
		ReleaseObject(frontCulling);
		ReleaseObject(disableCulling);
		ReleaseObject(wireFrame);
	}

}