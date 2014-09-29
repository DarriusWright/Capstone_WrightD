#include "RayTracingDemo.h"

RTTI_DEFINITIONS(RayTracingDemo);


RayTracingDemo::RayTracingDemo(Game & game, Camera & camera) : DrawableGameComponent(game, camera),
mBlueColor(1.0f), mPosition(0, 0, 0), mLightPosition(0,0,-5)
{
}


RayTracingDemo::~RayTracingDemo()
{
}



void RayTracingDemo::initialize()
{
	SetCurrentDirectory(Utility::ExecutableDirectory().c_str());

	mEffect = new Effect(*mGame);
	mEffect->LoadCompiledEffect(L"RayTraceEffect.cso");
	mComputePass = mEffect->techniquesByName().at("compute")->passesByName().at("p0");
	mMaterial = new RayTracerMaterial();
	mMaterial->initialize(mEffect);
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = mGame->screenWidth();
	textureDesc.Height = mGame->screenHeight();
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr;
	ID3D11Texture2D * texture = nullptr;
	if (FAILED(hr = mGame->device()->CreateTexture2D(&textureDesc, nullptr, &texture)))
	{
		throw GameException("IDXGIDevice::CreateTexture2D() failed.", hr);
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));

	uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	if (FAILED(hr = mGame->device()->CreateUnorderedAccessView(texture, &uavDesc, &mOutputTexture)))
	{
		ReleaseObject(texture);
		throw GameException("IDXGIDevice::CreateUnorderedAccessView() failed.", hr);

	}

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDescription;
	ZeroMemory(&resourceViewDescription, sizeof(resourceViewDescription));
	resourceViewDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resourceViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	resourceViewDescription.Texture2D.MipLevels = 1;

	if (FAILED(hr = mGame->device()->CreateShaderResourceView(texture, &resourceViewDescription, &mColorTexture)))
	{
		ReleaseObject(texture);
		throw GameException("IDXGIDevice::CreateShaderResourceView() failed", hr);
	}

	ReleaseObject(texture);
	mTextureSize = { static_cast<float>(mGame->screenWidth()), static_cast<float>(mGame->screenHeight()) };
	mThreadGroupCount = { mTextureSize.x / THREADS_PER_GROUP, mTextureSize.y / THREADS_PER_GROUP };

	mFullScreenQuad = new FullScreenQuad(*mGame, *mMaterial);
	mFullScreenQuad->SetActiveTechnique("render", "p0");
	mFullScreenQuad->SetCustomUpdateMaterial(std::bind(&RayTracingDemo::updateRenderingMaterial, this));
	mFullScreenQuad->initialize();

	mKeyboard = (Keyboard*)mGame->getServiceContainer().getService(Keyboard::typeIdClass());
}
void RayTracingDemo::update(const GameTimer & timer)
{
	mBlueColor = (0.5f) * static_cast<float>(sin(timer.totalGameTime())) + 0.5f;

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
		
		if (mKeyboard->isKeyDown(DIK_K))
		{
			movementAmount.y = 1.0f;
		}

		if (mKeyboard->isKeyDown(DIK_I))
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

		float elapsedTime = (float)timer.elapsedGameTime();
		XMVECTOR position = XMLoadFloat3(&mPosition) + (XMLoadFloat3(&movementAmount) * 100.0f * timer.elapsedGameTime());
		XMStoreFloat3(&mPosition, position);
		
	}

	XMFLOAT3 lightMovementAmount = { 0, 0, 0 };
	if (mKeyboard != nullptr)
	{
		if (mKeyboard->isKeyDown(DIK_A))
		{
			lightMovementAmount.x = -1.0f;
		}

		if (mKeyboard->isKeyDown(DIK_D))
		{
			lightMovementAmount.x = 1.0f;
		}

		if (mKeyboard->isKeyDown(DIK_W))
		{
			lightMovementAmount.y = 1.0f;
		}

		if (mKeyboard->isKeyDown(DIK_S))
		{
			lightMovementAmount.y = -1.0f;

		}

		if (mKeyboard->isKeyDown(DIK_R))
		{
			lightMovementAmount.z = -1.0f;

		}

		if (mKeyboard->isKeyDown(DIK_F))
		{
			lightMovementAmount.z = 1.0f;

		}

		float elapsedTime = (float)timer.elapsedGameTime();

		XMVECTOR lightPosition = XMLoadFloat3(&mLightPosition) + (XMLoadFloat3(&lightMovementAmount) * 20.0f* timer.elapsedGameTime());
		XMStoreFloat3(&mLightPosition, lightPosition);
	}

	



}
void RayTracingDemo::draw(const GameTimer & timer)
{
	mTextureSize = { static_cast<float>(mGame->screenWidth()), static_cast<float>(mGame->screenHeight()) };

	ID3D11DeviceContext * deviceContext = mGame->deviceContext();
	mMaterial->TextureSize() << XMLoadFloat2(&mTextureSize);
	mMaterial->BlueColor() << mBlueColor;
	mMaterial->CameraPosition() << mCamera->positionVector();
	mMaterial->InverseViewMatrix() << XMMatrixInverse(nullptr,mCamera->viewMatrix());
	mMaterial->InverseProjectionMatrix() << XMMatrixInverse(nullptr, mCamera->projectionMatrix());
	mMaterial->ViewMatrix() << mCamera->viewMatrix();
	mMaterial->Position() << XMLoadFloat3(&mPosition);
	mMaterial->LightPosition() << XMLoadFloat3(&mLightPosition);
	mMaterial->ProjectionMatrix() << mCamera->projectionMatrix();
	//mMaterial->SpherePosition() << mPosition;

	mMaterial->OutputTexture() << mOutputTexture;
	mComputePass->apply(0, deviceContext);

	deviceContext->Dispatch(mThreadGroupCount.x, mThreadGroupCount.y, 1);
	static ID3D11UnorderedAccessView * emptyUAV = nullptr;
	deviceContext->CSSetUnorderedAccessViews(0, 1, &emptyUAV, nullptr);

	mFullScreenQuad->draw(timer);
}

void RayTracingDemo::updateRenderingMaterial()
{
	mMaterial->ColorTexture() << mColorTexture;
}

