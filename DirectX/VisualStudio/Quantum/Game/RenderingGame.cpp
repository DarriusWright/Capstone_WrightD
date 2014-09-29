#include "RenderingGame.h"
#include <Exceptions\GameException.h>
#include <DataManagement\MeshManager.h>
namespace Rendering
{
	const XMVECTORF32 RenderingGame::BackgroundColor = { 0.7f, 0.2f, 0.3f, 1.0f };

	RenderingGame::RenderingGame(HINSTANCE instance, const std::wstring& windowClass, const std::wstring & windowTitle, int showCommand)
		:Game(instance, windowClass, windowTitle, showCommand), mFpsComponent(nullptr),
		mTextComponent(nullptr), mKeyboard(nullptr), mMouse(nullptr)//, mDemo(nullptr)
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
		DeleteObject(mDemo);
		//DeleteObject(diffuseDemo);
		DeleteObject(pointDemo);

		DeleteObject(mSkybox);
		//DeleteObject(mMeshManager);
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
		mTextComponent = new TextComponent(*this, L"This is my text");
		mTextComponent->setTextPosition({ 50, 100 });
		mKeyboard = new Keyboard(*this, mDirectInput);
		mMouse = new Mouse(*this, mDirectInput);
		mCamera = new FirstPersonCamera(*this);
		mSkybox = new Skybox(*this, *mCamera, L"Content/Textures/Maskonaive2_1024.dds", 500.0f);
		//mDemo = new TriangleDemo(*this, *mCamera);
		//mDemo = new CubeDemo(*this, *mCamera);
		//mDemo = new ModelManagementDemo(*this, *mCamera);
		mDemo = new MaterialDemo(*this, *mCamera);
		//diffuseDemo = new DiffuseLightingDemo(*this, *mCamera);
		pointDemo = new PointLightDemo(*this, *mCamera);

		mServiceContainer.addService(Keyboard::typeIdClass(), mKeyboard);
		mServiceContainer.addService(Mouse::typeIdClass(), mMouse);
		mServiceContainer.addService(FirstPersonCamera::typeIdClass(),mCamera);

	
		mComponents.push_back(mSkybox);
		mComponents.push_back(mCamera);
		mComponents.push_back(pointDemo);
		//mComponents.push_back(diffuseDemo);
		mComponents.push_back(mDemo);
		//mComponents.push_back(mDemo2);
		mComponents.push_back(mMouse);
		mComponents.push_back(mKeyboard);
		mComponents.push_back(mFpsComponent);
		mComponents.push_back(mTextComponent);

		//mMeshManager = new MeshManager(*this,*mCamera, mDemo->getInputLayout(), mDemo->getPass(), mDemo->getWvpVariable());
		//mMeshManager->initialize();
		
		Game::initialize();
		//mMeshManager->setDrawInformation(mDemo->getInputLayout(), mDemo->getPass(), mDemo->getWvpVariable());

		//mServiceContainer.addService(MeshManager::typeIdClass(), mMeshManager);
		//mComponents.push_back(mMeshManager);


		mCamera->setPosition(0.0f, 0.0f, 25.0f);

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