#include "ModelDemo.h"
#include <3DObjects\Model.h>
#include <3DObjects\Mesh.h>
#include <Window\Game.h>
#include <DataManagement\MeshManager.h>

namespace Library
{
	RTTI_DEFINITIONS(ModelDemo);


	ModelDemo::ModelDemo(Game & game, Camera & camera) : DrawableGameComponent(game, camera),
		mEffect(nullptr), mTechnique(nullptr), mPass(nullptr), mWvpVariable(nullptr), mInputLayout(nullptr),
		mVertexBuffer(nullptr), mWorldMatrix(XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f)), mAngle(0.0f)
	{
	}


	ModelDemo::~ModelDemo()
	{
		ReleaseObject(mPass);
		ReleaseObject(mTechnique);
		ReleaseObject(mWvpVariable);
		ReleaseObject(mInputLayout);

		ReleaseObject(mEffect);
		ReleaseObject(mVertexBuffer);
		ReleaseObject(mIndexBuffer);
	}


	void ModelDemo::update(const GameTimer & timer)
	{
		mCamera->update(timer);
		//mAngle += XM_PI * static_cast<float>(timer.elapsedGameTime());
		//XMStoreFloat4x4(&mWorldMatrix, XMMatrixRotationY(mAngle));
	}

	void ModelDemo::draw(const GameTimer & timer)
	{
		ID3D11DeviceContext * deviceContext = mGame->deviceContext();
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->IASetInputLayout(mInputLayout);

		UINT stride = sizeof(BasicEffectVertex);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX worldMatrix = XMLoadFloat4x4(&mWorldMatrix);
		XMMATRIX wvp = worldMatrix * mCamera->viewMatrix() * mCamera->projectionMatrix();
		mWvpVariable->SetMatrix(reinterpret_cast<float*>(&wvp));

		mPass->Apply(0, deviceContext);
		deviceContext->DrawIndexed(mIndexCount, 0, 0);

	}

	void ModelDemo::initialize()
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


		//std::unique_ptr<Model> model(new Model(*mGame, L"Content/Models/Sphere.obj",L"dfgfds" ,true));
		//Mesh * mesh = model->meshes().at(0);
		////mGame->meshManager()->addMesh(mesh);
		//createVertexBuffer(mGame->device(), *mesh, &mVertexBuffer);
		//mesh->createIndexBuffer(&mIndexBuffer);
		//mIndexCount = mesh->indices().size();


		/*BasicEffectVertex vertices[] =
		{
			BasicEffectVertex(XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT4(reinterpret_cast<float*>(&Colors::RED))),
			BasicEffectVertex(XMFLOAT4(1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT4(reinterpret_cast<float*>(&Colors::GREEN))),
			BasicEffectVertex(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(reinterpret_cast<float*>(&Colors::BLACK))),

			BasicEffectVertex(XMFLOAT4(-1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(reinterpret_cast<float*>(&Colors::WHITE))),
			BasicEffectVertex(XMFLOAT4(-1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT4(reinterpret_cast<float*>(&Colors::YELLOW))),
			BasicEffectVertex(XMFLOAT4(+1.0f, -1.0f, +1.0f, 1.0f), XMFLOAT4(reinterpret_cast<float*>(&Colors::BLACK))),
			BasicEffectVertex(XMFLOAT4(1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT4(reinterpret_cast<float*>(&Colors::CYAN))),
			BasicEffectVertex(XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT4(reinterpret_cast<float*>(&Colors::GREEN)))
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

		UINT indices[] =
		{
			0, 1, 2,
			0, 2, 3,

			4, 5, 6,
			4, 6, 7,

			3, 2, 5,
			3, 5, 4,

			2, 1, 6,
			2, 6, 5,

			1, 7, 6,
			1, 0, 7,

			0, 3, 4,
			0, 4, 7,
		};

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.ByteWidth = sizeof(indices);
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexSubResourceData;
		ZeroMemory(&indexSubResourceData, sizeof(indexSubResourceData));
		indexSubResourceData.pSysMem = indices;
		if (FAILED(mGame->device()->CreateBuffer(&indexBufferDesc, &indexSubResourceData, &mIndexBuffer)))
		{
			throw GameException(L"ID3D11Device::CreateBuffer() failed");
		}*/

		mCamera->setPosition(0.0f, 0.0f, 5.0f);
	}




	void ModelDemo::createVertexBuffer(ID3D11Device * device,Mesh & mesh, ID3D11Buffer ** vertexBuffer)const
	{
		const std::vector<XMFLOAT3>& sourceVertices = mesh.vertices();
		std::vector<BasicEffectVertex> vertices;
		vertices.reserve(sourceVertices.size());
		if (mesh.vertexColors().size() > 0)
		{
			std::vector<XMFLOAT4>* vertexColors = mesh.vertexColors().at(0);
			assert(vertexColors->size() == sourceVertices.size());

			for (UINT i = 0; i < sourceVertices.size(); i++)
			{
				XMFLOAT3 position = sourceVertices.at(i);
				XMFLOAT4 color = vertexColors->at(i);
				vertices.push_back(BasicEffectVertex(XMFLOAT4(position.x, position.y, position.z, 1.0f), color));
			}
			
		}
		else
		{
			for (UINT i = 0; i < sourceVertices.size(); i++)
			{
				XMFLOAT3 position = sourceVertices.at(i);
				XMFLOAT4 color(Colors::CYAN);
				vertices.push_back(BasicEffectVertex(XMFLOAT4(position.x, position.y, position.z, 1.0f), color));
			}
		}

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.ByteWidth = sizeof(BasicEffectVertex)* vertices.size();
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexSubResourceData;
		ZeroMemory(&vertexSubResourceData, sizeof(vertexSubResourceData));
		vertexSubResourceData.pSysMem = &vertices[0];
		if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, vertexBuffer)))
		{
			throw GameException(L"ID3D11Device::CreateBuffer() failed");
		}

	}


}
