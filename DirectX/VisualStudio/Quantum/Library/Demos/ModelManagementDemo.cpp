#include "ModelManagementDemo.h"
#include <3DObjects\Model.h>
#include <3DObjects\Mesh.h>
#include <Window\Game.h>
#include <d3d11.h>
#include <DataManagement\MeshManager.h>
#include <WICTextureLoader.h>

namespace Library
{
	RTTI_DEFINITIONS(ModelManagementDemo);


	ModelManagementDemo::ModelManagementDemo(Game & game, Camera & camera) : DrawableGameComponent(game, camera),
		mEffect(nullptr), mTechnique(nullptr), mPass(nullptr), mWvpVariable(nullptr), mInputLayout(nullptr),
		mVertexBuffer(nullptr), mWorldMatrix(XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f)), mAngle(0.0f)
	{
	}


	ID3DX11EffectPass * ModelManagementDemo::getPass()
	{
		return mPass;
	}
	ID3DX11EffectMatrixVariable * ModelManagementDemo::getWvpVariable()
	{
		return mWvpVariable;
	}
	ID3D11InputLayout * ModelManagementDemo::getInputLayout()
	{
		return mInputLayout;
	}

	ModelManagementDemo::~ModelManagementDemo()
	{
		ReleaseObject(mPass);
		ReleaseObject(mTechnique);
		ReleaseObject(mWvpVariable);
		ReleaseObject(mInputLayout);

		ReleaseObject(mEffect);
		DeleteObject(model);
		DeleteObject(model2);
		DeleteObject(model3);
	}


	void ModelManagementDemo::update(const GameTimer & timer)
	{
		mCamera->update(timer);
	}

	void ModelManagementDemo::draw(const GameTimer & timer)
	{
		//mGame->meshManager()->draw(timer);
	}

	void ModelManagementDemo::initialize()
	{
		SetCurrentDirectory(Utility::ExecutableDirectory().c_str());
		UINT shaderFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		ID3D10Blob * compiledShader = nullptr;
		ID3D10Blob * errorMessages = nullptr;
		HRESULT hr = D3DCompileFromFile(L"Content\\Effects\\BinaryAlpha.fx", nullptr, nullptr, nullptr, "fx_5_0", shaderFlags, 0, &compiledShader, &errorMessages);
		//HRESULT hr = D3DCompileFromFile(L"Content\\Effects\\BasicEffect.fx", nullptr, nullptr, nullptr, "fx_5_0", shaderFlags, 0, &compiledShader, &errorMessages);
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

		variable = mEffect->GetVariableByName("ColorTexture");

		if (variable == nullptr)
		{
			throw GameException(L"ID3DX11Effect::GetVariableByName() unable to find variable ColorTexture");
		}

		mColorTextureVarialble = variable->AsShaderResource();
		if (!mColorTextureVarialble->IsValid())
		{
			throw GameException("Invalid effect variable cast.");
		}

		D3DX11_PASS_DESC passDesc;
		mPass->GetDesc(&passDesc);

		//D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] =
		//{
		//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		//};

		D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		};

		if (FAILED(hr = mGame->device()->CreateInputLayout(inputElementDescriptions, ARRAYSIZE(inputElementDescriptions), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mInputLayout)))
		{
			throw GameException(L"ID3D11Device::CreateInputLayout() failed", hr);
		}

		model = new Model(*mGame, "Content/Models/cube.obj",mEffect, L"Content/Textures/leaf.png", true);
		model2 = new Model(*mGame, "Content/Models/Sphere.obj",mEffect, L"Content/Textures/EarthComposite.jpg", true);
		model3 = new Model(*mGame, "Content/Models/cube.obj",mEffect, L"Content/Textures/leaf.jpg", true);

		//Mesh * outputMesh2 = mGame->meshManager()->meshes()[0];
		//Mesh * outputMesh3 = mGame->meshManager()->meshes()[1];
		//outputMesh2->left();
		//outputMesh3->right();

		//mGame->meshManager()->mColorTextureVarialble = mColorTextureVarialble;
		//mGame->meshManager()->mTextureShaderResourceView = mTextureShaderResourceView;

		mCamera->setPosition(0.0f, 0.0f, 5.0f);
	}


}
