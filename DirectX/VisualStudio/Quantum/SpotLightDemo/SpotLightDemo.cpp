#include "SpotLightDemo.h"
#include <Misc\MatrixHelper.h>
#include <3DObjects\Model.h>
#include <3DObjects\Mesh.h>
#include <Effects\Effect.h>
#include <Effects\Pass.h>
#include <Materials\SpotLightMaterial.h>
#include <Input\Keyboard.h>
#include <Input\Mouse.h>
#include <Lights\SpotLight.h>
#include <ProxyModel.h>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

RTTI_DEFINITIONS(SpotLightDemo);

const float SpotLightDemo::MOVEMENT_RATE = UCHAR_MAX;
const XMFLOAT2 SpotLightDemo::ROTATION_RATE = XMFLOAT2(XM_2PI, XM_2PI);

SpotLightDemo::SpotLightDemo(Game & game, Camera & camera) : DrawableGameComponent(game,camera)
,  mEffect(nullptr), mMaterial(nullptr), mTextureShaderResourceView(nullptr),
mVertexBuffer(nullptr), mIndexBuffer(nullptr), mIndexCount(0),
mKeyboard(nullptr), mAmbientColor(1, 1, 1, 0), mSpecularColor(1, 1, 1, 1), mSpecularPower(25.0f), mSpotLight(nullptr),
mWorldMatrix(MatrixHelper::INDENTITY), mProxyModel(nullptr)

{
}

SpotLightDemo::~SpotLightDemo()
{

	DeleteObject(mProxyModel);
	DeleteObject(mSpotLight);
	ReleaseObject(mTextureShaderResourceView);
	DeleteObject(mMaterial);
	DeleteObject(mEffect);
	ReleaseObject(mVertexBuffer);
	ReleaseObject(mIndexBuffer);
}

void SpotLightDemo::initialize()
{
	SetCurrentDirectory(Utility::ExecutableDirectory().c_str());

	std::unique_ptr<Model> model(new Model(*mGame, "Content/Models/Sphere.obj", true));

	// Initialize the material
	mEffect = new Effect(*mGame);
	mEffect->LoadCompiledEffect(L"SpotLight.cso");

	mMaterial = new SpotLightMaterial();
	mMaterial->initialize(mEffect);

	Mesh* mesh = model->meshes().at(0);
	mMaterial->createVertexBuffer(mGame->device(), *mesh, &mVertexBuffer);
	mesh->createIndexBuffer(&mIndexBuffer);
	mIndexCount = mesh->indices().size();

	std::wstring textureName = L"Content/Textures/Earthatday.dds";
	HRESULT hr = DirectX::CreateDDSTextureFromFile(mGame->device(), textureName.c_str(), nullptr, &mTextureShaderResourceView);
	if (FAILED(hr))
	{
		throw GameException("CreateDDSTextureFromFile() failed.", hr);
	}


	mSpotLight = new SpotLight(*mGame);
	mSpotLight->setRadius(500.0f);
	mSpotLight->setPosition(XMFLOAT3{ 0.0f, 0.0f, 10.0f });

	mKeyboard = (Keyboard*)mGame->getServiceContainer().getService(Keyboard::typeIdClass());
	assert(mKeyboard != nullptr);

	mProxyModel = new ProxyModel(*mGame, *mCamera, "Content/Models/DirectionalLightProxy.obj", 0.5f);
	mProxyModel->initialize();
	mProxyModel->setPosition(XMFLOAT3(0.0f, 0.0, 10.0f));
	mProxyModel->applyRotation(XMMatrixRotationY(- (90.0f * 3.14)/ 180.0f));
	//mSpotLight->applyRotation(XMMatrixRotationY(XM_PIDIV2));
	mProxyModel->initialize();

}


void SpotLightDemo::draw(const GameTimer & gameTimer)
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

	XMMATRIX worldMatrix = XMLoadFloat4x4(&mWorldMatrix);
	XMMATRIX wvp = worldMatrix * mCamera->viewMatrix() * mCamera->projectionMatrix();
	XMVECTOR ambientColor = XMLoadColor(&mAmbientColor);
	XMVECTOR specularColor = XMLoadColor(&mSpecularColor);
	XMVECTOR cameraPosition = mCamera->positionVector();


	mMaterial->WorldViewProjection() << wvp;
	mMaterial->World() << worldMatrix;
	mMaterial->SpecularColor() << specularColor;
	mMaterial->SpecularPower() << mSpecularPower;
	mMaterial->AmbientColor() << ambientColor;
	mMaterial->LightColor() << mSpotLight->colorVector();
	mMaterial->LightPosition() << mSpotLight->positionVector();
	mMaterial->LightLookAt() << mSpotLight->directionVector();
	mMaterial->LightRadius() << mSpotLight->radius();
	mMaterial->ColorTexture() << mTextureShaderResourceView;
	mMaterial->SpotLightInnerAngle() << mSpotLight->innerAngle();
	mMaterial->SpotLightOuterAngle() << mSpotLight->outerAngle();
	mMaterial->CameraPosition() << mCamera->positionVector();

	pass->apply(0, deviceContext);
	deviceContext->DrawIndexed(mIndexCount, 0, 0);

	mProxyModel->draw(gameTimer);
}

void SpotLightDemo::update(const GameTimer & gameTime)
{
	updateAmbientLight(gameTime);
	updateSpotLight(gameTime);

	mProxyModel->update(gameTime);
}

void SpotLightDemo::updateSpecular(const GameTimer & gameTime)
{
	static float specularIntensity = mSpecularPower;

	if (mKeyboard != nullptr)
	{
		if (mKeyboard->isKeyDown(DIK_INSERT) && specularIntensity < UCHAR_MAX)
		{
			specularIntensity += MOVEMENT_RATE * (float)gameTime.elapsedGameTime();
			specularIntensity = XMMin<float>(specularIntensity, UCHAR_MAX);

			mSpecularPower = specularIntensity;;
		}

		if (mKeyboard->isKeyDown(DIK_DELETE) && specularIntensity > 0)
		{
			specularIntensity -= MOVEMENT_RATE * (float)gameTime.elapsedGameTime();
			specularIntensity = XMMax<float>(specularIntensity, 0);

			mSpecularPower = specularIntensity;
		}
	}
}


void SpotLightDemo::updateSpotLight(const GameTimer& gameTime)
{



	XMFLOAT3 movementAmount = { 0, 0, 0 };
	if (mKeyboard != nullptr)
	{
		if (mKeyboard->isKeyDown(DIK_J))
		{
			movementAmount.x = -1.0f;
		}

		if (mKeyboard->isKeyDown(DIK_L))
		{
			movementAmount.x = 1.0f;
		}

		if (mKeyboard->isKeyDown(DIK_I))
		{
			movementAmount.y = 1.0f;
		}

		if (mKeyboard->isKeyDown(DIK_K))
		{
			movementAmount.y = -1.0f;

		}

		if (mKeyboard->isKeyDown(DIK_U))
		{
			movementAmount.z = -1.0f;

		}

		if (mKeyboard->isKeyDown(DIK_O))
		{
			movementAmount.z = 1.0f;

		}
		static float directionalIntensity = mSpotLight->color().a;
		float elapsedTime = (float)gameTime.elapsedGameTime();
		XMFLOAT2 rotationAmount = { 0, 0 };
		if (mKeyboard->isKeyDown(DIK_LEFTARROW))
		{
			rotationAmount.x += ROTATION_RATE.x * elapsedTime;
		}
		if (mKeyboard->isKeyDown(DIK_RIGHTARROW))
		{
			rotationAmount.x -= ROTATION_RATE.x * elapsedTime;
		}
		if (mKeyboard->isKeyDown(DIK_UPARROW))
		{
			rotationAmount.y += ROTATION_RATE.y * elapsedTime;
		}
		if (mKeyboard->isKeyDown(DIK_DOWNARROW))
		{
			rotationAmount.y -= ROTATION_RATE.y * elapsedTime;
		}

		XMMATRIX lightRotationMatrix = XMMatrixIdentity();
		if (rotationAmount.x != 0)
		{
			lightRotationMatrix = XMMatrixRotationY(rotationAmount.x);
		}

		if (rotationAmount.y != 0)
		{
			XMMATRIX lightRotationAxisMatrix = XMMatrixRotationAxis(mSpotLight->rightVector(), rotationAmount.y);
			lightRotationMatrix *= lightRotationAxisMatrix;
		}

		if (rotationAmount.x != 0.0f || rotationAmount.y != 0.0f)
		{
			mSpotLight->applyRotation(lightRotationMatrix);
			mProxyModel->applyRotation(lightRotationMatrix);
		}


	}

	XMVECTOR movement = XMLoadFloat3(&movementAmount)  *  20.0f * (float)gameTime.elapsedGameTime();
	mSpotLight->setPosition(mSpotLight->positionVector() + movement);
	mProxyModel->setPosition(mSpotLight->position());
}

void SpotLightDemo::updateAmbientLight(const GameTimer& gameTime)
{
	static float ambientIntensity = mAmbientColor.a;

	if (mKeyboard != nullptr)
	{
		if (mKeyboard->isKeyDown(DIK_PGUP) && ambientIntensity < UCHAR_MAX)
		{
			ambientIntensity += MOVEMENT_RATE * (float)gameTime.elapsedGameTime();
			mAmbientColor.a = (UCHAR)XMMin<float>(ambientIntensity, UCHAR_MAX);
		}

		if (mKeyboard->isKeyDown(DIK_PGDN) && ambientIntensity > 0)
		{
			ambientIntensity -= MOVEMENT_RATE * (float)gameTime.elapsedGameTime();
			mAmbientColor.a = (UCHAR)XMMax<float>(ambientIntensity, 0);
		}
	}
}
