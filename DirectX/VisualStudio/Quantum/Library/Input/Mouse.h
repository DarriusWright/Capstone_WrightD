#pragma once

#include <dinput.h>
#include <Misc\Common.h>
#include <Exceptions\GameException.h>
#include <Window\Game.h>

namespace Library
{
	enum MouseButtons
	{
		MouseButtonsLeft = 0,
		MouseButtonsRight = 1,
		MouseButtonsMiddle = 2,
		MouseButtonsX1 = 3
	};

	class Mouse : public GameComponent
	{

		RTTI_DECLARATIONS(Mouse, GameComponent)
	public:
		Mouse(Game & game, LPDIRECTINPUT8 directInput);
		~Mouse();
		LPDIMOUSESTATE currentState();
		LPDIMOUSESTATE lastState();
		virtual void initialize() override;
		virtual void update(const GameTimer & gameTime) override;
		long x()const;
		long y()const;
		long wheel()const;

		bool isButtonUp(MouseButtons button)const;
		bool isButtonDown(MouseButtons button)const;
		bool wasButtonUp(MouseButtons button)const;
		bool wasButtonDown(MouseButtons button)const;
		bool wasButtonPressedThisFrame(MouseButtons button)const;
		bool wasButtonReleasedThisFrame(MouseButtons button)const;
		bool isButtonHeldDown(MouseButtons button)const;

	private:
		Mouse();
		LPDIRECTINPUT8 mDirectInput;
		DIMOUSESTATE mCurrentState;
		DIMOUSESTATE mLastState;
		//LPDIRECTINPUTDEVICEW mDevice;
		LPDIRECTINPUTDEVICE8 mDevice;

		
		long mX;
		long mY;
		long mWheel;

	};

}