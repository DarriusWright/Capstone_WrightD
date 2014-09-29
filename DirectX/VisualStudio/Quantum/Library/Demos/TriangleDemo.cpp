#include "TriangleDemo.h"

namespace Library
{
	RTTI_DEFINITIONS(TriangleDemo);


	TriangleDemo::TriangleDemo(Game & game, Camera & camera) : DrawableGameComponent(game, camera),
		mEffect(nullptr), mTechnique(nullptr), mPass(nullptr), mWvpVariable(nullptr), mInputLayout(nullptr),
		mVertexBuffer(nullptr), mWorldMatrix(XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f)), mAngle(0.0f)
	{
	}


	TriangleDemo::~TriangleDemo()
	{
		ReleaseObject(mEffect);
		ReleaseObject(mPass);
		ReleaseObject(mTechnique);
		ReleaseObject(mInputLayout);
		ReleaseObject(mWvpVariable);
		ReleaseObject(mVertexBuffer)
	}


	void TriangleDemo::update(const GameTimer & timer)
	{
		mCamera->update(timer);
		mAngle += XM_PI * static_cast<float>(timer.elapsedGameTime());
		XMStoreFloat4x4(&mWorldMatrix, XMMatrixRotationZ(mAngle));
	}

	void TriangleDemo::draw(const GameTimer & timer)
	{
		ID3D11DeviceContext * deviceContext = mGame->deviceContext();
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->IASetInputLayout(mInputLayout);

		UINT stride = sizeof(BasicEffectVertex);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

		XMMATRIX worldMatrix = XMLoadFloat4x4(&mWorldMatrix);
		XMMATRIX wvp = worldMatrix * mCamera->viewMatrix() * mCamera->projectionMatrix();
		mWvpVariable->SetMatrix(reinterpret_cast<float*>(&wvp));

		mPass->Apply(0, deviceContext);
		deviceContext->Draw(3, 0);

		//ID3D11DeviceContext* direct3DDeviceContext = mGame->deviceContext();
		//direct3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//direct3DDeviceContext->IASetInputLayout(mInputLayout);

		//UINT stride = sizeof(BasicEffectVertex);
		//UINT offset = 0;
		//direct3DDeviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

		//XMMATRIX worldMatrix = XMLoadFloat4x4(&mWorldMatrix);
		//XMMATRIX wvp = worldMatrix * mCamera->viewMatrix() * mCamera->projectionMatrix();
		//mWvpVariable->SetMatrix(reinterpret_cast<const float*>(&wvp));

		//mPass->Apply(0, direct3DDeviceContext);

		//direct3DDeviceContext->Draw(3, 0);
	}

	void TriangleDemo::initialize()
	{
		SetCurrentDirectory(Utility::ExecutableDirectory().c_str());
		UINT shaderFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		ID3D10Blob * compiledShader = nullptr;
		ID3D10Blob * errorMessages = nullptr;
		HRESULT hr = D3DCompileFromFile(L"Content\\Effects\\BasicEffect.fx", nullptr, nullptr, nullptr, "fx_5_0", shaderFlags, 0, &compiledShader, &errorMessages);
		//if (errorMessages != nullptr)
		//{
		//	char * message = (char*)errorMessages->GetBufferPointer();
		//	GameException ex((wchar_t*)errorMessages->GetBufferPointer(), hr);
		//	ReleaseObject(errorMessages);
		//	throw ex;
		//	//ReleaseObject(compiledShader);
		//}

		if (FAILED(hr))
		{
			throw GameException(L"D3DX11CompileFromFile() failed.", hr);
		}

		hr = D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, mGame->device(), &mEffect);

		if (FAILED(hr))
		{
			throw GameException(L"D3DX11CreateEffectFromMemory() failed", hr);
		}

		ReleaseObject(compiledShader);

		mTechnique = mEffect->GetTechniqueByName("main11");

		if (mTechnique == nullptr)
		{
			throw GameException(L"ID3D11Effect::GetTechniqueByName() unable to find techique main11.", hr);
		}

		mPass = mTechnique->GetPassByName("p0");
		if (mPass == nullptr)
		{
			throw GameException(L"ID3D11EffectTechnique::GetPassByName() unable to find pass p0", hr);
		}

		ID3DX11EffectVariable * variable = mEffect->GetVariableByName("WorldViewProjection");
		if (variable == nullptr)
		{
			throw GameException(L"ID3DX11Effect::GetVariableByName() unable to find variable WorldViewProjection");
		}

		mWvpVariable = variable->AsMatrix();

		if (!mWvpVariable->IsValid())
		{
			throw GameException(L"Invaild effect variable cast");
		}

		D3DX11_PASS_DESC passDesc;
		mPass->GetDesc(&passDesc);

		D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		};

		if (FAILED(hr = mGame->device()->CreateInputLayout(inputElementDescriptions, ARRAYSIZE(inputElementDescriptions), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mInputLayout)))
		{
			throw GameException(L"ID3D11Device::CreateInputLayout() failed", hr);
		}





		BasicEffectVertex vertices[] =
		{
			BasicEffectVertex(XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(reinterpret_cast<float*>(&Colors::RED))),
			BasicEffectVertex(XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(reinterpret_cast<float*>(&Colors::GREEN))),
			BasicEffectVertex(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(reinterpret_cast<float*>(&Colors::BLUE)))
		};

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.ByteWidth = sizeof(BasicEffectVertex)* ARRAYSIZE(vertices);
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexSubResourceData;
		ZeroMemory(&vertexSubResourceData, sizeof(vertexSubResourceData));
		vertexSubResourceData.pSysMem = vertices;
		if (FAILED(mGame->device()->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, &mVertexBuffer)))
		{
			throw GameException(L"ID3D11Device::CreateBuffer() failed");
		}

		mCamera->setPosition(0.0f, 0.0f, 5.0f);
	}

}
