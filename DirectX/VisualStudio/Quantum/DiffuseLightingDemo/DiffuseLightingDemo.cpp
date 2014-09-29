#include "DiffuseLightingDemo.h"
#include <Effects\Pass.h>
#include <Misc\MatrixHelper.h>
#include <WICTextureLoader.h>
#include <3DObjects\Model.h>
#include <3DObjects\Mesh.h>
namespace Library
{

	RTTI_DEFINITIONS(DiffuseLightingDemo);


	const float DiffuseLightingDemo::AMBIENT_MODULATION_RATE = UCHAR_MAX;
	const XMFLOAT2 DiffuseLightingDemo::ROTATION_RATE = XMFLOAT2(XM_2PI, XM_2PI);

	DiffuseLightingDemo::DiffuseLightingDemo(Game & game, Camera & camera) : DrawableGameComponent(game,camera)
		, mEffect(nullptr), mMaterial(nullptr), mTextureShaderResourceView(nullptr),
		mVertexBuffer(nullptr), mIndexBuffer(nullptr), mIndexCount(0),
		mKeyboard(nullptr), mAmbientColor(1, 1, 1, 0), mDirectionalLight(nullptr),
		mWorldMatrix(MatrixHelper::INDENTITY), mProxyModel(nullptr)

	
	{
	}


	DiffuseLightingDemo::~DiffuseLightingDemo()
	{
		DeleteObject(mRenderStateHelper);
		DeleteObject(mProxyModel);
		DeleteObject(mDirectionalLight);
		ReleaseObject(mTextureShaderResourceView);
		DeleteObject(mMaterial);
		DeleteObject(mEffect);
		ReleaseObject(mVertexBuffer);
		ReleaseObject(mIndexBuffer);
	}

	void DiffuseLightingDemo::initialize()
	{
		SetCurrentDirectory(Utility::ExecutableDirectory().c_str());

		std::unique_ptr<Model> model(new Model(*mGame, "Content/Models/Sphere.obj", true));

		// Initialize the material
		mEffect = new Effect(*mGame);
		mEffect->LoadCompiledEffect(L"DiffuseLighting.cso");

		mMaterial = new DiffuseLightingMaterial();
		mMaterial->initialize(mEffect);

		Mesh* mesh = model->meshes().at(0);
		mMaterial->createVertexBuffer(mGame->device(), *mesh, &mVertexBuffer);
		mesh->createIndexBuffer(&mIndexBuffer);
		mIndexCount = mesh->indices().size();

		std::wstring textureName = L"Content/Textures/EarthComposite.jpg";
		HRESULT hr = DirectX::CreateWICTextureFromFile(mGame->device(), mGame->deviceContext(), textureName.c_str(), nullptr, &mTextureShaderResourceView);
		if (FAILED(hr))
		{
			throw GameException("CreateWICTextureFromFile() failed.", hr);
		}

		mDirectionalLight = new DirectionalLight(*mGame);

		mKeyboard = (Keyboard*)mGame->getServiceContainer().getService(Keyboard::typeIdClass());
		assert(mKeyboard != nullptr);

		mProxyModel = new ProxyModel(*mGame, *mCamera, "Content/Models/DirectionalLightProxy.obj", 0.5f);
		mProxyModel->initialize();
		mProxyModel->setPosition(XMFLOAT3( 10.0f, 0.0, 0.0f ));
		mProxyModel->applyRotation(XMMatrixRotationY(XM_PIDIV2));


		mRenderStateHelper = new RenderStateHelper(*mGame);

	}


	void DiffuseLightingDemo::draw(const GameTimer & gameTimer)
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

		mMaterial->World() << worldMatrix;
		mMaterial->WorldViewProjection() << wvp;
		mMaterial->AmbientColor() << ambientColor;
		mMaterial->LightColor() << mDirectionalLight->colorVector();
		mMaterial->LightDirection() << mDirectionalLight->directionVector();
		mMaterial->ColorTexture() << mTextureShaderResourceView;

		pass->apply(0, deviceContext);
		deviceContext->DrawIndexed(mIndexCount, 0, 0);
		mProxyModel->draw(gameTimer);

		mRenderStateHelper->restoreAll();
	}

	void DiffuseLightingDemo::update(const GameTimer & gameTime)
	{
		updateAmbientLight(gameTime);
		updateDirectionalLight(gameTime);

		mProxyModel->update(gameTime);
	}
	//redo
	void DiffuseLightingDemo::updateDirectionalLight(const GameTimer& gameTime)
	{
		static float directionalIntensity = mDirectionalLight->color().a;
		float elapsedTime = (float)gameTime.elapsedGameTime();

		// Update directional light intensity		
		if (mKeyboard->isKeyDown(DIK_HOME) && directionalIntensity < UCHAR_MAX)
		{
			directionalIntensity += AMBIENT_MODULATION_RATE * elapsedTime;

			XMCOLOR directionalLightColor = mDirectionalLight->color();
			directionalLightColor.a = (UCHAR)XMMin<float>(directionalIntensity, UCHAR_MAX);
			mDirectionalLight->setColor(directionalLightColor);
		}
		if (mKeyboard->isKeyDown(DIK_END) && directionalIntensity > 0)
		{
			directionalIntensity -= AMBIENT_MODULATION_RATE * elapsedTime;

			XMCOLOR directionalLightColor = mDirectionalLight->color();
			directionalLightColor.a = (UCHAR)XMMax<float>(directionalIntensity, 0.0f);
			mDirectionalLight->setColor(directionalLightColor);
		}

		// Rotate directional light
		XMFLOAT2 rotationAmount = {0,0};
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
			XMMATRIX lightRotationAxisMatrix = XMMatrixRotationAxis(mDirectionalLight->rightVector(), rotationAmount.y);
			lightRotationMatrix *= lightRotationAxisMatrix;
		}

		if (rotationAmount.x != 0.0f || rotationAmount.y != 0.0f)
		{
			mDirectionalLight->applyRotation(lightRotationMatrix);
			mProxyModel->applyRotation(lightRotationMatrix);
		}
	}

	void DiffuseLightingDemo::updateAmbientLight(const GameTimer& gameTime)
	{
		static float ambientIntensity = mAmbientColor.a;

		if (mKeyboard != nullptr)
		{
			if (mKeyboard->isKeyDown(DIK_PGUP) && ambientIntensity < UCHAR_MAX)
			{
				ambientIntensity += AMBIENT_MODULATION_RATE * (float)gameTime.elapsedGameTime();
				mAmbientColor.a = (UCHAR)XMMin<float>(ambientIntensity, UCHAR_MAX);
			}

			if (mKeyboard->isKeyDown(DIK_PGDN) && ambientIntensity > 0)
			{
				ambientIntensity -= AMBIENT_MODULATION_RATE * (float)gameTime.elapsedGameTime();
				mAmbientColor.a = (UCHAR)XMMax<float>(ambientIntensity, 0);
			}
		}
	}

}