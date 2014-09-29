#include "Game.h"
#include <Exceptions\GameException.h>
#include <DataManagement\MeshManager.h>
#include <Misc\RasterizerStates.h>

namespace Library
{


	const UINT Game::DEFAULT_SCREEN_HEIGHT = 640;
	const UINT Game::DEFAULT_SCREEN_WIDTH = 480;
	const UINT Game::DEFAULT_FRAMERATE = 60;
	const UINT Game::DEFAULT_MULTISAMPLING_COUNT = 4;

	Game::Game(HINSTANCE windowInstance, const  std::wstring & windowClass, const  std::wstring & windowName, int command) :
		mInstance(windowInstance), mWindowClass(windowClass), mWindowTitle(windowName), mShowCommand(command),
		mScreenWidth(DEFAULT_SCREEN_WIDTH), mScreenHeight(DEFAULT_SCREEN_HEIGHT), mMultiSamplingCount(DEFAULT_MULTISAMPLING_COUNT), mFrameRate(DEFAULT_FRAMERATE)
		, mIsFullScreen(false), mWindowHandle(), mWindow(), mGameClock(), mGameTime(), mSelectedLevel(D3D_FEATURE_LEVEL_9_1),
		mDevice(NULL), mDeviceContext(NULL), mDepthBufferEnabled(true), mMultiSamplingEnabled(true)
	{
	}


	Game::~Game()
	{

	}

	//MeshManager * Game::meshManager()const
	//{
	//	return mMeshManager;
	//}

	void Game::exit()
	{
		PostQuitMessage(0);
	}

	void Game::initializeDirectX()
	{
		HRESULT hr;
		UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};

		ID3D11DeviceContext * deviceContext;
		ID3D11Device * device;

		DXThrowIfFailed(L"D3D11CreateDevice() failed", hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &device, &mSelectedLevel, &deviceContext));

		DXThrowIfFailed(L"ID3D11Device::QueryInterface() failed", hr = device->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&mDevice)));

		DXThrowIfFailed(L"ID3D11DeviceContext::QueryInterface() failed", hr = deviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&mDeviceContext)));


		mDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, mMultiSamplingCount, &mMultiSamplingQualityLevels);

		//if (mMultiSamplingEnabled)
		//{
		//	throw GameException(L"Unsupported multi-sampling quality");
		//}

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
		swapChainDesc.Width = mScreenWidth;
		swapChainDesc.Height = mScreenHeight;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		if (mMultiSamplingEnabled)
		{
			swapChainDesc.SampleDesc.Count = mMultiSamplingCount;
			swapChainDesc.SampleDesc.Quality = mMultiSamplingQualityLevels - 1;
		}
		else
		{
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
		}

		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc;
		ZeroMemory(&fullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
		fullScreenDesc.RefreshRate.Numerator = mFrameRate;
		fullScreenDesc.RefreshRate.Denominator = 1;
		fullScreenDesc.Windowed = !mIsFullScreen;

		IDXGIDevice * dxgiDevice = nullptr;
		DXThrowIfFailed(L"D3D11Device::QueryInterface() failed", hr = mDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice)));

		IDXGIAdapter * dxgiAdapter = nullptr;
		if (FAILED(hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter))))
		{
			ReleaseObject(dxgiDevice);
			throw GameException(L"IDXGIDevice::GetParent() failed retrieving adapter.", hr);
		}

		IDXGIFactory2 * dxgiFactory = nullptr;
		if (FAILED(hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory))))
		{
			ReleaseObject(dxgiDevice);
			ReleaseObject(dxgiAdapter);
			throw GameException(L"IDXGIAdapter::GetParent() failed retrieving.", hr);
		}


		if (FAILED(hr = dxgiFactory->CreateSwapChainForHwnd(dxgiDevice, mWindowHandle, &swapChainDesc, &fullScreenDesc, nullptr, &mSwapChain)))
		{
			ReleaseObject(dxgiDevice);
			ReleaseObject(dxgiAdapter);
			ReleaseObject(dxgiFactory);
			throw GameException(L"IDXGIDevice::CreateSwapChainForHwnd() failed.", hr);

		}

		ReleaseObject(dxgiDevice);
		ReleaseObject(dxgiAdapter);
		ReleaseObject(dxgiFactory);

		ID3D11Texture2D * backBuffer;

		DXThrowIfFailed(L"IDGXSwapChain::GetBuffer() failed.", hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));

		if (FAILED(hr = mDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView)))
		{
			ReleaseObject(backBuffer);
			throw GameException(L"IDXGIDevice::Create RenderTargetView() failed.", hr);
		}

		ReleaseObject(backBuffer);

		D3D11_TEXTURE2D_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.Width = mScreenWidth;
		depthStencilDesc.Height = mScreenHeight;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;

		if (mMultiSamplingEnabled)
		{
			depthStencilDesc.SampleDesc.Count = mMultiSamplingCount;
			depthStencilDesc.SampleDesc.Quality = mMultiSamplingQualityLevels - 1;
		}
		else
		{
			depthStencilDesc.SampleDesc.Count = 1;
			depthStencilDesc.SampleDesc.Quality = 0;
		}

		if (FAILED(hr = mDevice->CreateTexture2D(&depthStencilDesc, nullptr, &mDepthStencilBuffer)))
		{
			throw GameException(L"IDXGIDevice::CreateTexture2D() failed", hr);
		}

		if (FAILED(hr = mDevice->CreateDepthStencilView(mDepthStencilBuffer, nullptr, &mDepthStencilView)))
		{
			throw GameException(L"IDXGIDevice::CreateDepthStencilView() failed", hr);
		}

		mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

		mViewport.TopLeftX = 0.0f;
		mViewport.TopLeftY = 0.0f;
		mViewport.Width = static_cast<float>(mScreenWidth);
		mViewport.Height = static_cast<float>(mScreenHeight);
		mViewport.MinDepth = 0.0f;
		mViewport.MaxDepth = 1.0f;

		mDeviceContext->RSSetViewports(1, &mViewport);
	}

	void Game::run()
	{
		initialize();
		MSG message;
		ZeroMemory(&message, sizeof(message));
		mGameClock.reset();
		while (message.message != WM_QUIT)
		{
			if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
			else
			{
				mGameClock.update(mGameTime);
				update(mGameTime);
				draw(mGameTime);
			}

			//mGameClock.update(mGameTime);
		}

		shutdown();


	}

	void Game::update(const  Library::GameTimer & gameTime)
	{
		for (GameComponent * component : mComponents)
		{
			if (component->enabled())
			{
				component->update(gameTime);
			}
		}
	}
	void Game::draw(const  Library::GameTimer & gameTime)
	{
		for (GameComponent * component : mComponents)
		{
			DrawableGameComponent * drawableGameComponent = component->as<DrawableGameComponent>();
			if (drawableGameComponent != nullptr && drawableGameComponent->visible())
			{
				mRenderStateHelper->saveAll();
				drawableGameComponent->draw(gameTime);
				mRenderStateHelper->restoreAll();
			}
		}
	}
	void Game::initialize()
	{
		mRenderStateHelper = new RenderStateHelper(*this);
		/*mMeshManager = new MeshManager(*this);
		mComponents.push_back(mMeshManager);
		mServiceContainer.addService(MeshManager::typeIdClass(), mMeshManager);*/
		initializeWindow();
		initializeDirectX();
		RasterizerStates::initialize(mDevice);
		for (GameComponent * component : mComponents)
		{
			component->initialize();
		}


	}
	HINSTANCE Game::instance()const
	{
		return mInstance;
	}
	HWND Game::windowHandle()const
	{
		return mWindowHandle;
	}
	const WNDCLASSEX & Game::window()const
	{
		return mWindow;
	}
	const std::wstring & Game::windowClass()const
	{
		return mWindowClass;
	}
	const std::wstring & Game::windowTitle()const
	{
		return mWindowTitle;
	}
	int Game::screenWidth()const
	{
		return mScreenWidth;
	}
	int Game::screenHeight()const
	{
		return mScreenHeight;
	}




	const ServiceContainer & Game::getServiceContainer() const
	{
		return mServiceContainer;
	}

	float Game::aspectRatio()const
	{
		return static_cast<float>(mScreenWidth)/mScreenHeight;
	}

	void Game::initializeWindow()
	{
		ZeroMemory(&mWindow, sizeof(mWindow));
		mWindow.cbSize = sizeof(WNDCLASSEX);
		mWindow.style = CS_CLASSDC;
		mWindow.lpfnWndProc = wndProc;
		mWindow.hInstance = mInstance;
		mWindow.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		mWindow.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
		mWindow.hCursor = LoadCursor(nullptr, IDC_ARROW);
		mWindow.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
		mWindow.lpszClassName = mWindowClass.c_str();

		RECT windowRectangle = { 0, 0, mScreenWidth, mScreenHeight };
		AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);
		RegisterClassEx(&mWindow);
		POINT center = centerWindow(mScreenWidth, mScreenHeight);
		mWindowHandle = CreateWindow(mWindowClass.c_str(), mWindowTitle.c_str(), WS_OVERLAPPEDWINDOW, center.x, center.y, windowRectangle.right - windowRectangle.left, windowRectangle.bottom - windowRectangle.top, nullptr, nullptr, mInstance, nullptr);
		ShowWindow(mWindowHandle, mShowCommand);
		UpdateWindow(mWindowHandle);
	}

	const std::vector<GameComponent*>& Game::components()const
	{
		return mComponents;
	}


	void Game::shutdown()
	{
		ReleaseObject(mRenderTargetView);
		ReleaseObject(mSwapChain);
		ReleaseObject(mDepthStencilBuffer);
		ReleaseObject(mDepthStencilView);
		if (mDeviceContext != nullptr)
		{
			mDeviceContext->ClearState();
		}

		ReleaseObject(mDeviceContext);
		ReleaseObject(mDevice);
		DeleteObject(mRenderStateHelper);
		RasterizerStates::release();
		UnregisterClass(mWindowClass.c_str(), mWindow.hInstance);
	}

	LRESULT WINAPI Game::wndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc(windowHandle, message, wParam, lParam);


	}

	POINT Game::centerWindow(int windowWidth, int windowHeight)
	{
		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		POINT center;
		center.x = (screenWidth - windowWidth) / 2;
		center.y = (screenHeight - windowHeight) / 2;

		return center;
	}


	bool Game::depthBufferEnabled()const
	{
		return mDepthBufferEnabled;
	}
	bool Game::isFullScreen()const
	{
		return mIsFullScreen;
	}
	ID3D11Device1 * Game::device()const
	{
		return mDevice;
	}
	ID3D11DeviceContext1 * Game::deviceContext()const
	{
		return mDeviceContext;
	}
	const D3D11_TEXTURE2D_DESC & Game::backBufferDesc()const
	{
		return mBackBufferDesc;
	}
	const D3D11_VIEWPORT& Game::viewPort()const
	{
		return mViewport;
	}

}