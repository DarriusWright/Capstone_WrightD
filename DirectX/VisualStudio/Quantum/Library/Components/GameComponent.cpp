#include "GameComponent.h"

namespace Library
{
	RTTI_DEFINITIONS(GameComponent)
	GameComponent::GameComponent() : mGame(nullptr), mEnabled(true)
	{
	}
	GameComponent::GameComponent(Game & game) : mGame(&game), mEnabled(true)
	{

	}

	GameComponent::~GameComponent() 
	{
	}

	Game * GameComponent::getGame()
	{
		return mGame;
	}
	void GameComponent::setGame(Game & game)
	{
		mGame = &game;
	}
	bool GameComponent::enabled()const
	{
		return mEnabled;
	}
	void GameComponent::setEnabled(bool enabled)
	{
		mEnabled = enabled;
	}
	void GameComponent::initialize()
	{

	}
	void GameComponent::update(const GameTimer & gameTime)
	{

	}
}