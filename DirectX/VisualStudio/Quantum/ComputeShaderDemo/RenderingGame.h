#pragma once
#include <Window\Game.h>
#include <Misc\Common.h>
#include <Components\DrawableGameComponent.h>
#include <Components\FpsComponent.h>
#include <Components\TextComponent.h>
#include <Input\Keyboard.h>
#include <Input\Mouse.h>
#include <dinput.h>
#include <string.h>
#include <Demos\TriangleDemo.h>
#include <Demos\CubeDemo.h>
#include <Demos\ModelDemo.h>
#include <Demos\ModelManagementDemo.h>
#include <Camera\FirstPersonCamera.h>
#include <Demos\MaterialDemo.h>
#include <Components\Skybox.h>
#include "ComputeShaderDemo.h"
#include <Demos\PointLightDemo.h>

using namespace Library;
namespace Rendering
{
	
	class RenderingGame : public Game
	{
	public:
		RenderingGame(HINSTANCE instance, const std::wstring& windowClass, const std::wstring & windowTitle, int showCommand);
		~RenderingGame();
		virtual void initialize() override;
		virtual void update(const GameTimer & gameTime)override;
		virtual void draw(const GameTimer & gameTime)override;
		virtual void shutdown()override;
	private:
		static const XMVECTORF32 BackgroundColor;
		FpsComponent * mFpsComponent;
		TextComponent * mTextComponent;
		Keyboard * mKeyboard;
		Mouse * mMouse;
		FirstPersonCamera * mCamera;
		
		ComputeShaderDemo * mComputeDemo;
	protected:
		LPDIRECTINPUT8 mDirectInput;
	};

}