#include "PointLightDemo.h"
#include <Effects\Pass.h>
#include <Misc\MatrixHelper.h>
#include <WICTextureLoader.h>
#include <3DObjects\Model.h>
#include <3DObjects\Mesh.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <sstream>
#include <Misc\VectorHelper.h>

#include <DDSTextureLoader.h>
namespace Library
{

	RTTI_DEFINITIONS(PointLightDemo);


	const float PointLightDemo::MOVEMENT_RATE = UCHAR_MAX;
	const XMFLOAT2 PointLightDemo::ROTATION_RATE = XMFLOAT2(XM_2PI, XM_2PI);

	PointLightDemo::PointLightDemo(Game & game, Camera & camera) : DrawableGameComponent(game, camera)
		, mEffect(nullptr), mMaterial(nullptr), mTextureShaderResourceView(nullptr),
		mVertexBuffer(nullptr), mIndexBuffer(nullptr), mIndexCount(0),
		mKeyboard(nullptr), mAmbientColor(1, 1, 1, 0),mSpecularColor(1,1,1,1),mSpecularPower(25.0f), mPointLight(nullptr),
		mWorldMatrix(MatrixHelper::INDENTITY), mProxyModel(nullptr)


	{
	}


	PointLightDemo::~PointLightDemo()
	{
		
		DeleteObject(mProxyModel);
		DeleteObject(mPointLight);
		ReleaseObject(mTextureShaderResourceView);
		DeleteObject(mMaterial);
		DeleteObject(mEffect);
		ReleaseObject(mVertexBuffer);
		ReleaseObject(mIndexBuffer);
	}

	void PointLightDemo::initialize()
	{
		SetCurrentDirectory(Utility::ExecutableDirectory().c_str());

		std::unique_ptr<Model> model(new Model(*mGame, "Content/Models/Sphere.obj", true));

		// Initialize the material
		mEffect = new Effect(*mGame);
		mEffect->LoadCompiledEffect(L"PointLight.cso");

		mMaterial = new PointLightMaterial();
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


		mPointLight = new PointLight(*mGame);
		mPointLight->setRadius(500.0f);
		mPointLight->setPosition(XMFLOAT3{ 5.0f, 0.0f, 10.0f });

		mKeyboard = (Keyboard*)mGame->getServiceContainer().getService(Keyboard::typeIdClass());
		assert(mKeyboard != nullptr);

		mProxyModel = new ProxyModel(*mGame, *mCamera, "Content/Models/PointLightProxy.obj", 0.5f);
		mProxyModel->initialize();

	}


	void PointLightDemo::draw(const GameTimer & gameTimer)
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
		mMaterial->LightColor() << mPointLight->colorVector();
		mMaterial->LightPosition() << mPointLight->positionVector();
		mMaterial->LightRadius() << mPointLight->radius();
		mMaterial->ColorTexture() << mTextureShaderResourceView;
		mMaterial->CameraPosition() << mCamera->positionVector();

		pass->apply(0, deviceContext);
		deviceContext->DrawIndexed(mIndexCount, 0, 0);

		mProxyModel->draw(gameTimer);
	}

	void PointLightDemo::update(const GameTimer & gameTime)
	{
		updateAmbientLight(gameTime);
		updatePointLight(gameTime);

		mProxyModel->update(gameTime);
	}
	//redo
	void PointLightDemo::updatePointLight(const GameTimer& gameTime)
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


		}

		XMVECTOR movement = XMLoadFloat3(&movementAmount)  *  20.0f * (float)gameTime.elapsedGameTime();
		mPointLight->setPosition(mPointLight->positionVector() + movement);
		mProxyModel->setPosition(mPointLight->position());
	}

	void PointLightDemo::updateAmbientLight(const GameTimer& gameTime)
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

}