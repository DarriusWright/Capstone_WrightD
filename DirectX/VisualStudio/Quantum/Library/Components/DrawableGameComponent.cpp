#include "DrawableGameComponent.h"

namespace Library
{

	RTTI_DEFINITIONS(DrawableGameComponent)

	DrawableGameComponent::DrawableGameComponent()
	{

	}
	DrawableGameComponent::DrawableGameComponent(Game & game) : GameComponent(game)
	{

	}
	DrawableGameComponent::DrawableGameComponent(Game & game, Camera & camera) : GameComponent(game), mCamera(&camera)
	{

	}
	DrawableGameComponent::~DrawableGameComponent()
	{

	}

	bool DrawableGameComponent::visible()const
	{
		return mVisible;
	}
	void DrawableGameComponent::setVisible(bool isVisible)
	{
		mVisible = isVisible;
	}
	Camera * DrawableGameComponent::getCamera()
	{
		return mCamera;
	}
	void DrawableGameComponent::setCamera(Camera * camera)
	{
		mCamera = camera;
	}
	void DrawableGameComponent::draw(const GameTimer & gameTime)
	{
	}
}