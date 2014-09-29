#pragma once
#include <dinput.h>
#include <Misc\Common.h>
#include <Exceptions\GameException.h>
#include <Window\Game.h>

namespace Library
{

	class Keyboard :public GameComponent
	{
		RTTI_DECLARATIONS(Keyboard, GameComponent);

	public:
		Keyboard(Game & game, LPDIRECTINPUT8 directInput);
		~Keyboard();
		virtual void initialize() override;
		virtual void update(const GameTimer & gameTime) override;

		
		const byte* const currentState()const;
		const byte* const lastState()const;

		bool isKeyUp(byte key) const;
		bool isKeyDown(byte key)const;
		bool wasKeyUp(byte key)const;
		bool wasKeyDown(byte key)const;
		bool wasKeyPressedThisFrame(byte key)const;
		bool wasKeyReleasedThisFrame(byte key)const;
		bool isKeyHeldDown(byte key)const;
	private:
		Keyboard();
		Keyboard(const Keyboard & rhs);
		Camera& operator=(const Keyboard & rhs);
		static const int KEY_COUNT = 256;

		LPDIRECTINPUT8 mDirectInput;
		LPDIRECTINPUTDEVICE8 mDevice;

		byte mCurrentState[KEY_COUNT];
		byte mLastState[KEY_COUNT];

	};

}