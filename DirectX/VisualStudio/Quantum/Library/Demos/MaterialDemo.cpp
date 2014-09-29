#include "MaterialDemo.h"
#include <Window\Game.h>
#include <Exceptions\GameException.h>
#include <Camera\Camera.h>
#include <Misc\Common.h>
#include <Misc\Utility.h>
#include <3DObjects\Model.h>
#include <3DObjects\Mesh.h>
#include <Effects\Pass.h>
#include <Materials\BasicMaterial.h>

RTTI_DEFINITIONS(MaterialDemo);

MaterialDemo::MaterialDemo(Game & game, Camera & camera) : DrawableGameComponent(game, camera), mBasicMaterial(nullptr), mBasicEffect(nullptr), 
mWorldMatrix(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -20, 1), mVertexBuffer(nullptr), mIndexBuffer(nullptr), mIndexCount(0)
{
}


MaterialDemo::~MaterialDemo()
{
	DeleteObject(mBasicMaterial);
	DeleteObject(mBasicEffect);
	ReleaseObject(mVertexBuffer);
	ReleaseObject(mIndexBuffer);
}

void MaterialDemo::initialize()
{
	SetCurrentDirectory(Utility::ExecutableDirectory().c_str());

	std::unique_ptr<Model> model(new Model(*mGame, "Content/Models/Sphere.obj", true));
	mBasicEffect = new Effect(*mGame);
	mBasicEffect->LoadCompiledEffect(L"BasicEffect.cso");
	mBasicMaterial = new BasicMaterial();
	mBasicMaterial->initialize(mBasicEffect);

	Mesh * mesh = model->meshes().at(0);
	mBasicMaterial->createVertexBuffer(mGame->device(), *mesh, &mVertexBuffer);
	mesh->createIndexBuffer(&mIndexBuffer);
	mIndexCount = mesh->indices().size();
}

void MaterialDemo::draw(const GameTimer & gameTime)
{
	ID3D11DeviceContext * deviceContext = mGame->deviceContext();
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Pass * pass = mBasicMaterial->currentTechnique()->passes().at(0);
	ID3D11InputLayout * inputLayout = mBasicMaterial->inputLayouts().at(pass);
	deviceContext->IASetInputLayout(inputLayout);

	UINT stride = mBasicMaterial->vertexSize();
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	XMMATRIX worldMatrix = XMLoadFloat4x4(&mWorldMatrix);
	XMMATRIX wvp = worldMatrix * mCamera->viewMatrix() * mCamera->projectionMatrix();
	mBasicMaterial->WorldViewProjection() << wvp;
	pass->apply(0, deviceContext);

	deviceContext->DrawIndexed(mIndexCount, 0,0);
}
