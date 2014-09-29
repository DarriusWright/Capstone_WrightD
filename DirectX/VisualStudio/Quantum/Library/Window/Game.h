#pragma once
#include <string>
#include <Timer\GameTimer.h>
#include <Timer\GameClock.h>
#include <Misc\Common.h>
#include <Components\GameComponent.h>
#include <Components\DrawableGameComponent.h>
#include <Services\ServiceContainer.h>
#include <Misc\RenderStateHelper.h>

namespace Library
{

	class DrawableGameComponent;
	class GameComponent;
	class MeshManager;
	class Game
	{
	public:
		Game(HINSTANCE windowInstance, const std::wstring & windowClass, const  std::wstring & windowName, int command);
		~Game();
		virtual void run();
		virtual void exit();
		virtual void update(const  Library::GameTimer & gameTime);
		virtual void draw(const  Library::GameTimer & gameTime);
		virtual void initialize();
		float aspectRatio()const;


		bool depthBufferEnabled()const;
		bool isFullScreen()const;
		ID3D11Device1 * device()const;
		ID3D11DeviceContext1 * deviceContext()const;
		//MeshManager * meshManager()const;

		const D3D11_TEXTURE2D_DESC & backBufferDesc()const;
		const D3D11_VIEWPORT& viewPort()const;

		HINSTANCE instance()const;
		HWND windowHandle()const;
		const WNDCLASSEX & window()const;
		const std::wstring & windowClass()const;
		const std::wstring & windowTitle()const;
		int screenWidth()const;
		int screenHeight()const;

		std::wstring windowTitle();
		const std::vector<GameComponent*>& components()const;
		const ServiceContainer & getServiceContainer() const;
	private:
		Game(const Game& rhs);
		Game& operator=(const Game& rhs);
		
		POINT centerWindow(int windowWidth, int windowHeight);
		static LRESULT WINAPI wndProc(HWND windowHandle, UINT message, WPARAM, LPARAM lParam);
	protected:
		RenderStateHelper * mRenderStateHelper;
		//MeshManager * mMeshManager;
		ServiceContainer mServiceContainer;
		std::vector<GameComponent *> mComponents;
		virtual void initializeWindow();
		virtual void initializeDirectX();
		virtual void shutdown();

		static const UINT DEFAULT_SCREEN_WIDTH;
		static const UINT DEFAULT_SCREEN_HEIGHT;
		static const UINT DEFAULT_FRAMERATE;
		static const UINT DEFAULT_MULTISAMPLING_COUNT;

		HINSTANCE mInstance;
		std::wstring mWindowClass;
		std::wstring mWindowTitle;
		int mShowCommand;

		HWND mWindowHandle;
		WNDCLASSEX mWindow;
		UINT mScreenWidth;
		UINT mScreenHeight;
		Library::GameTimer mGameTime;
		Library::GameClock mGameClock;

		ID3D11DeviceContext1 * mDeviceContext;
		ID3D11Device1 * mDevice;
		IDXGISwapChain1 * mSwapChain;
		ID3D11RenderTargetView *mRenderTargetView;
		ID3D11Texture2D * mDepthStencilBuffer;
		ID3D11DepthStencilView * mDepthStencilView;
		D3D11_VIEWPORT mViewport;
		D3D11_TEXTURE2D_DESC mBackBufferDesc;
		bool mDepthBufferEnabled;

		D3D_FEATURE_LEVEL mSelectedLevel;
		UINT mMultiSamplingCount;
		UINT mMultiSamplingQualityLevels;
		bool mMultiSamplingEnabled;
		USHORT mFrameRate;
		bool mIsFullScreen;
	};

}