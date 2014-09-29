#include "RayTracingDemo.h"

RTTI_DEFINITIONS(RayTracingDemo);


const float RayTracingDemo::CLEAR_COLOR[4] = { 0.1f,0.4f,0.8f,1.0f };
//const int RayTracingDemo::NUM_SPHERE = 5;
const float RayTracingDemo::CLEAR_DEPTH[4] = {0.0f,0.0f,0.0f,0.0f};
float const RayTracingDemo::MAX_POSITION = 300.0f;
float const RayTracingDemo::MIN_POSITION = -300.0f;
const unsigned int RayTracingDemo::THREADS_PER_GROUP = 16;

RayTracingDemo::RayTracingDemo(Game & game, PinholeCamera & camera) : DrawableGameComponent(game, camera),
mBlueColor(1.0f), mPosition(0, 0, 0), mLightPosition(0, 0, -5), mCamera(&camera)
{
}


RayTracingDemo::~RayTracingDemo()
{
}




void RayTracingDemo::initialize()
{
	SetCurrentDirectory(Utility::ExecutableDirectory().c_str());

	mEffect = new Effect(*mGame);
	mEffect->LoadCompiledEffect(L"ThinLensCameraRayTracing.cso");
	mComputePass = mEffect->techniquesByName().at("compute")->passesByName().at("p0");
	mMaterial = new RayTracerMaterial();
	mMaterial->initialize(mEffect);


	D3D11_TEXTURE2D_DESC colorTextureDesc;
	ZeroMemory(&colorTextureDesc, sizeof(colorTextureDesc));
	colorTextureDesc.Width = mGame->screenWidth();
	colorTextureDesc.Height = mGame->screenHeight();
	colorTextureDesc.MipLevels = 1;
	colorTextureDesc.ArraySize = 1;
	colorTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	colorTextureDesc.SampleDesc.Count = 1;
	colorTextureDesc.SampleDesc.Quality = 0;
	colorTextureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr;
	ID3D11Texture2D * colorTexture = nullptr;
	if (FAILED(hr = mGame->device()->CreateTexture2D(&colorTextureDesc, nullptr, &colorTexture)))
	{
		throw GameException(L"IDXGIDevice::CreateTexture2D() failed.", hr);
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavColorDesc;
	ZeroMemory(&uavColorDesc, sizeof(uavColorDesc));

	uavColorDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavColorDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavColorDesc.Texture2D.MipSlice = 0;

	if (FAILED(hr = mGame->device()->CreateUnorderedAccessView(colorTexture, &uavColorDesc, &mOutputTexture)))
	{
		ReleaseObject(colorTexture);
		throw GameException(L"IDXGIDevice::CreateUnorderedAccessView() failed.", hr);

	}

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDescription;
	ZeroMemory(&resourceViewDescription, sizeof(resourceViewDescription));
	resourceViewDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resourceViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	resourceViewDescription.Texture2D.MipLevels = 1;

	if (FAILED(hr = mGame->device()->CreateShaderResourceView(colorTexture, &resourceViewDescription, &mColorTexture)))
	{
		ReleaseObject(colorTexture);
		throw GameException(L"IDXGIDevice::CreateShaderResourceView() failed", hr);
	}

	//mTextureSize = { static_cast<float>(mGame->screenWidth()), static_cast<float>(mGame->screenHeight()) };
	//mThreadGroupCount = { mTextureSize.x / THREADS_PER_GROUP, mTextureSize.y / THREADS_PER_GROUP };


	D3D11_TEXTURE2D_DESC depthTextureDesc;
	ZeroMemory(&depthTextureDesc, sizeof(depthTextureDesc));
	depthTextureDesc.Width = mGame->screenWidth();
	depthTextureDesc.Height = mGame->screenHeight();
	depthTextureDesc.MipLevels = 1;
	depthTextureDesc.ArraySize = 1;
	//depthTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	depthTextureDesc.Format = DXGI_FORMAT_R32_FLOAT;
	depthTextureDesc.SampleDesc.Count = 1;
	depthTextureDesc.SampleDesc.Quality = 0;
	depthTextureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

	
	ID3D11Texture2D * depthTexture = nullptr;
	if (FAILED(hr = mGame->device()->CreateTexture2D(&depthTextureDesc, nullptr, &depthTexture)))
	{
		throw GameException(L"IDXGIDevice::CreateTexture2D() failed.", hr);
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDepthDesc;
	ZeroMemory(&uavDepthDesc, sizeof(uavDepthDesc));

	//uavDepthDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavDepthDesc.Format = DXGI_FORMAT_R32_FLOAT;
	uavDepthDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDepthDesc.Texture2D.MipSlice = 0;

	if (FAILED(hr = mGame->device()->CreateUnorderedAccessView(depthTexture, &uavDepthDesc, &mDepthTexture)))
	{
		ReleaseObject(depthTexture);
		throw GameException(L"IDXGIDevice::CreateUnorderedAccessView() failed.", hr);

	}

	//D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDescription;
	//ZeroMemory(&resourceViewDescription, sizeof(resourceViewDescription));
	//resourceViewDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//resourceViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	//resourceViewDescription.Texture2D.MipLevels = 1;

	//if (FAILED(hr = mGame->device()->CreateShaderResourceView(depthTexture, &resourceViewDescription, &mColorTexture)))
	//{
	//	ReleaseObject(depthTexture);
	//	throw GameException("IDXGIDevice::CreateShaderResourceView() failed", hr);
	//}
	ReleaseObject(colorTexture);

	ReleaseObject(depthTexture);
	mTextureSize = { static_cast<float>(mGame->screenWidth()), static_cast<float>(mGame->screenHeight()) };
	mThreadGroupCount = { mTextureSize.x / THREADS_PER_GROUP, mTextureSize.y / THREADS_PER_GROUP };

	mFullScreenQuad = new FullScreenQuad(*mGame, *mMaterial);
	mFullScreenQuad->SetActiveTechnique("render", "p0");
	mFullScreenQuad->SetCustomUpdateMaterial(std::bind(&RayTracingDemo::updateRenderingMaterial, this));
	mFullScreenQuad->initialize();

	mKeyboard = (Keyboard*)mGame->getServiceContainer().getService(Keyboard::typeIdClass());


	for (size_t i = 0; i < NUM_SPHERE; i++)
	{
		XMFLOAT3 position(getRandomFloat(MIN_POSITION, MAX_POSITION), getRandomFloat(MIN_POSITION, MAX_POSITION), getRandomFloat(-300.0f,0));
		spheres[i] = RayTraceSphere(*mGame, *mCamera, position);
		spheres[i].setColor(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));

	}

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
		if (mKeyboard->isKeyDown(DIK_LEFTARROW))
		{
			lightMovementAmount.x = -1.0f;
		}

		if (mKeyboard->isKeyDown(DIK_RIGHTARROW))
		{
			lightMovementAmount.x = 1.0f;
		}

		if (mKeyboard->isKeyDown(DIK_DOWNARROW))
		{
			lightMovementAmount.y = 1.0f;
		}

		if (mKeyboard->isKeyDown(DIK_UPARROW))
		{
			lightMovementAmount.y = -1.0f;

		}

		if (mKeyboard->isKeyDown(DIK_PGDN))
		{
			lightMovementAmount.z = -1.0f;

		}

		if (mKeyboard->isKeyDown(DIK_PGUP))
		{
			lightMovementAmount.z = 1.0f;

		}

		float elapsedTime = (float)timer.elapsedGameTime();

		XMVECTOR lightPosition = XMLoadFloat3(&mLightPosition) + (XMLoadFloat3(&lightMovementAmount) * 100.0f* timer.elapsedGameTime());
		XMStoreFloat3(&mLightPosition, lightPosition);
	}

	



}
void RayTracingDemo::draw(const GameTimer & timer)
{
	mTextureSize = { static_cast<float>(mGame->screenWidth()), static_cast<float>(mGame->screenHeight()) };


	

	ID3D11DeviceContext * deviceContext = mGame->deviceContext();
	deviceContext->ClearUnorderedAccessViewFloat(mDepthTexture, CLEAR_DEPTH);
	deviceContext->ClearUnorderedAccessViewFloat(mOutputTexture, CLEAR_COLOR);

	mMaterial->TextureSize() << XMLoadFloat2(&mTextureSize);
	mMaterial->CameraPosition() << mCamera->positionVector();
	mMaterial->LightPosition() << XMLoadFloat3(&mLightPosition);
	mMaterial->U() << mCamera->uVector();
	mMaterial->V() << mCamera->vVector();
	mMaterial->W() << mCamera->wVector();
	mMaterial->Eye() << mCamera->eyeVector();
	mMaterial->Distance() << mCamera->viewDistance();

	for (size_t i = 0; i < NUM_SPHERE; i++)
	{

		mMaterial->Position() << spheres[i].positionVector() + XMLoadFloat3(&mPosition);

		mMaterial->DepthTexture() << mDepthTexture;
		mMaterial->OutputTexture() << mOutputTexture;
		mMaterial->Color() << spheres[i].colorVector();
		mComputePass->apply(0, deviceContext);

		deviceContext->Dispatch(mThreadGroupCount.x, mThreadGroupCount.y, 1);
		static ID3D11UnorderedAccessView * emptyUAV = nullptr;
		deviceContext->CSSetUnorderedAccessViews(0, 1, &emptyUAV, nullptr);

	}
	mFullScreenQuad->draw(timer);



}

void RayTracingDemo::updateRenderingMaterial()
{
	mMaterial->ColorTexture() << mColorTexture;
}

