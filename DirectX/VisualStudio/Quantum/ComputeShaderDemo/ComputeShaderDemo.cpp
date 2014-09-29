#include "ComputeShaderDemo.h"

RTTI_DEFINITIONS(ComputeShaderDemo);


ComputeShaderDemo::ComputeShaderDemo(Game & game, Camera & camera) : DrawableGameComponent(game, camera),
mBlueColor(1.0f)
{
}


ComputeShaderDemo::~ComputeShaderDemo()
{
}



void ComputeShaderDemo::initialize()
{
	SetCurrentDirectory(Utility::ExecutableDirectory().c_str());

	mEffect = new Effect(*mGame);
	mEffect->LoadCompiledEffect(L"ComputeShader.cso");
	mComputePass = mEffect->techniquesByName().at("compute")->passesByName().at("p0");
	mMaterial = new ComputeShaderMaterial();
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
	mFullScreenQuad->SetCustomUpdateMaterial(std::bind(&ComputeShaderDemo::updateRenderingMaterial, this));
	mFullScreenQuad->initialize();
}
void ComputeShaderDemo::update(const GameTimer & timer)
{
	mBlueColor = (0.5f) * static_cast<float>(sin(timer.totalGameTime())) + 0.5f;
}
void ComputeShaderDemo::draw(const GameTimer & timer)
{
	mTextureSize = { static_cast<float>(mGame->screenWidth()), static_cast<float>(mGame->screenHeight()) };

	ID3D11DeviceContext * deviceContext = mGame->deviceContext();
	mMaterial->TextureSize() << XMLoadFloat2(&mTextureSize);
	mMaterial->BlueColor() << mBlueColor;

	mMaterial->OutputTexture() << mOutputTexture;
	mComputePass->apply(0, deviceContext);

	deviceContext->Dispatch(mThreadGroupCount.x,mThreadGroupCount.y,1);
	static ID3D11UnorderedAccessView * emptyUAV = nullptr;
	deviceContext->CSSetUnorderedAccessViews(0, 1, &emptyUAV, nullptr);

	mFullScreenQuad->draw(timer);
}

void ComputeShaderDemo::updateRenderingMaterial()
{
	mMaterial->ColorTexture() << mColorTexture;
}

