#include "Skybox.h"
#include <Misc\Utility.h>
#include <3DObjects\Model.h>
#include <3DObjects\Mesh.h>
#include <DDSTextureLoader.h>
#include <Exceptions\GameException.h>
#include <Effects\Pass.h>
#include <Misc\MatrixHelper.h>

namespace Library
{

	RTTI_DEFINITIONS(Skybox)

	Skybox::Skybox(Game & game, Camera & camera, const std::wstring & cubeMapFile, float scale) : DrawableGameComponent(game, camera), mCubeMapFileName(cubeMapFile), 
		mEffect(nullptr), mMaterial(nullptr),
		mCubeMapShaderResourceView(nullptr), mVertexBuffer(nullptr), mIndexBuffer(nullptr), mIndexCount(0),
		mWorldMatrix(MatrixHelper::INDENTITY), mScaleMatrix(MatrixHelper::INDENTITY)
	{
		scale = 150;
		XMStoreFloat4x4(&mScaleMatrix, XMMatrixScaling(scale ,scale,scale));
	}



	void Skybox::initialize()
	{
		SetCurrentDirectory(Utility::ExecutableDirectory().c_str());

		std::unique_ptr<Model> model(new Model(*mGame, "Content/Models/Sphere.obj", true));
		mEffect = new Effect(*mGame);
		mEffect->LoadCompiledEffect(L"Skybox.cso");

		mMaterial = new SkyboxMaterial();
		mMaterial->initialize(mEffect);
		Mesh * mesh = model->meshes().at(0);
		mMaterial->createVertexBuffer(mGame->device(), *mesh, &mVertexBuffer);
		mesh->createIndexBuffer(&mIndexBuffer);
		mIndexCount = mesh->indices().size();

		HRESULT hr = DirectX::CreateDDSTextureFromFile(mGame->device(), mCubeMapFileName.c_str(), nullptr, &mCubeMapShaderResourceView);
		if (FAILED(hr))
		{
			throw GameException("CreateDDSTextureFromFile() failed", hr);
		}

	}

	Skybox::~Skybox()
	{
	}
	
	void Skybox::update(const GameTimer & gameTimer)
	{
		const XMFLOAT3 & position = mCamera->position();
		XMStoreFloat4x4(&mWorldMatrix, XMLoadFloat4x4(&mScaleMatrix) * XMMatrixTranslation(position.x, position.y, position.z));
	}
	void Skybox::draw(const GameTimer & gameTimer)
	{
		ID3D11DeviceContext * deviceContext = mGame->deviceContext();
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Pass * pass = mMaterial->currentTechnique()->passes().at(0);
		ID3D11InputLayout * inputLayout = mMaterial->inputLayouts().at(pass);
		deviceContext->IASetInputLayout(inputLayout);

		UINT stride = mMaterial->vertexSize();
		UINT offset = 0;

		deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX wvp = XMLoadFloat4x4(&mWorldMatrix) * mCamera->viewMatrix() * mCamera->projectionMatrix();

		mMaterial->WorldViewProjection() << wvp;
		mMaterial->SkyboxTexture() << mCubeMapShaderResourceView;

		pass->apply(0, deviceContext);
		deviceContext->DrawIndexed(mIndexCount, 0, 0);
	}



}