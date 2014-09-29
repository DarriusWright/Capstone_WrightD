#pragma once
#include <Misc\Common.h>

namespace Library
{
	class Game;
	class GameTimer;
	//class DrawableGameComponent;
	class GameComponent :public  RTTI
	{
		RTTI_DECLARATIONS(GameComponent, RTTI)
	public:
		GameComponent();
		GameComponent(Game & game);
		~GameComponent();
		Game * getGame();
		void setGame(Game & game);
		bool enabled()const;
		void setEnabled(bool enabled);
		virtual void initialize();
		virtual void update(const GameTimer & gameTime);
	private:
		//GameComponent(const GameComponent& rhs);
		//GameComponent & operator=(const GameComponent & rhs);
	protected:
		Game * mGame;
		bool mEnabled;
	};

}