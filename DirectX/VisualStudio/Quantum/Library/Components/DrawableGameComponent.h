#pragma once
#include "GameComponent.h"
#include <Camera\Camera.h>
namespace Library
{
	//class Game;
	//class GameComponent;
	//class DrawableGameComponent;
	class Camera;
	class DrawableGameComponent : public GameComponent
	{
		RTTI_DECLARATIONS(DrawableGameComponent, GameComponent)
	public:
		DrawableGameComponent();
		DrawableGameComponent(Game & game);
		DrawableGameComponent(Game & game, Camera & camera);
		virtual ~DrawableGameComponent();

		bool visible()const;
		void setVisible(bool isVisible);
		Camera * getCamera();
		void setCamera(Camera * camera);
		virtual void draw(const GameTimer & gameTime);
	protected:
		bool mVisible;
		Camera * mCamera;
	private:
	//	DrawableGameComponent(const DrawableGameComponent & rhs);
	//	DrawableGameComponent & operator=(const DrawableGameComponent & rhs);
	};

}