#include "RenderingGame.h"
#include <Exceptions\GameException.h>
#include <DataManagement\MeshManager.h>
namespace Rendering
{
	const XMVECTORF32 RenderingGame::BackgroundColor = { 0.7f, 0.2f, 0.3f, 1.0f };

	RenderingGame::RenderingGame(HINSTANCE instance, const std::wstring& windowClass, const std::wstring & windowTitle, int showCommand)
		:Game(instance, windowClass, windowTitle, showCommand), mFpsComponent(nullptr),
		mTextComponent(nullptr), mKeyboard(nullptr), mMouse(nullptr)
	{
		mDepthBufferEnabled = true;
		mMultiSamplingEnabled = true;
	}


	RenderingGame::~RenderingGame()
	{
	}

	void RenderingGame::shutdown()
	{
		DeleteObject(mKeyboard);
		DeleteObject(mMouse);
		DeleteObject(mFpsComponent);
		DeleteObject(mTextComponent);
		DeleteObject(mCamera);
		ReleaseObject(mDirectInput);
		

		Game::shutdown();
	}

	void RenderingGame::initialize()
	{
		if (FAILED(DirectInput8Create(mInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&mDirectInput,nullptr)))
		{
			throw GameException(L"DirectInput8Create() failed");
		}

		mFpsComponent = new FpsComponent(*this);
		mTextComponent = new TextComponent(*this, L"");
		mTextComponent->setTextPosition({ 0, 100 });
		mKeyboard = new Keyboard(*this, mDirectInput);
		mMouse = new Mouse(*this, mDirectInput);
		mCamera = new PinholeCamera(*this);
		mRayTracingDemo = new RayTracingDemo(*this, *mCamera);

		mServiceContainer.addService(Keyboard::typeIdClass(), mKeyboard);
		mServiceContainer.addService(Mouse::typeIdClass(), mMouse);
		mServiceContainer.addService(FirstPersonCamera::typeIdClass(),mCamera);

		mComponents.push_back(mCamera);
		mComponents.push_back(mRayTracingDemo);
		mComponents.push_back(mMouse);
		mComponents.push_back(mKeyboard);
		mComponents.push_back(mFpsComponent);
		mComponents.push_back(mTextComponent);
		
		Game::initialize();
		mCamera->setEyeVector(0, 0, 10);
		mCamera->setLookAt(0, 0, 0);
		mCamera->setViewDistance(500);
		

	}
	void RenderingGame::update(const GameTimer & gameTime)
	{
		if (mKeyboard->wasKeyPressedThisFrame(DIK_ESCAPE))
		{
			exit();
		}
		Game::update(gameTime);
	}

	void RenderingGame::draw(const GameTimer & gameTime)
	{
		mDeviceContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float *>(&BackgroundColor));
		mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		Game::draw(gameTime);
		
		HRESULT hr;
		
		hr = mSwapChain->Present(0, 0);

		std::wstring mouseString( L"MousePosition : " + std::to_wstring(mMouse->x()) );
		mouseString += L", " + std::to_wstring(mMouse->y());
		mouseString += L"\nMouseWheel " + std::to_wstring(mMouse->wheel());
		mTextComponent->setText(mouseString);

		if (FAILED(hr))
		{
			throw GameException(L"IDXGISwapChain::Present() failed.", hr);
		}
	}


}