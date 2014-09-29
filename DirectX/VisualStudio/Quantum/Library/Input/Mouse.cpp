#include "Mouse.h"

namespace Library
{

	RTTI_DEFINITIONS(Mouse);

	Mouse::Mouse(Game & game, LPDIRECTINPUT8 directInput) : GameComponent(game), mDirectInput(directInput)
		, mY(0), mX(0), mWheel(0), mDevice(nullptr)
	{

		assert(mDirectInput != nullptr);
		ZeroMemory(&mCurrentState, sizeof(mCurrentState));
		ZeroMemory(&mLastState, sizeof(mLastState));
	}


	Mouse::~Mouse()
	{
		if (mDevice != nullptr)
		{
			mDevice->Unacquire();
			mDevice->Release();
			mDevice = nullptr;
		}
	}

	LPDIMOUSESTATE Mouse::currentState()
	{
		return &mCurrentState;
	}
	LPDIMOUSESTATE Mouse::lastState()
	{
		return &mLastState;
	}

	long Mouse::x()const
	{
		return mX;
	}
	long Mouse::y()const
	{
		return mY;
	}
	long Mouse::wheel()const
	{
		return mWheel;
	}

	void Mouse::initialize()
	{
		if (FAILED(mDirectInput->CreateDevice(GUID_SysMouse, &mDevice, nullptr)))
		
		{
			throw GameException(L"IDIRECTINPUT8::CreateDevice() failed");
		}

		if (FAILED(mDevice->SetDataFormat(&c_dfDIMouse)))
		{
			throw GameException(L"IDIRECTINPUTDEVICE8::SetDataFormat() failed");
		}

		if (FAILED(mDevice->SetCooperativeLevel(mGame->windowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
		{
			throw GameException(L"IDIRECTINPUTDEVICE8::SetCoorperationLevel() failed");
		}

		mDevice->Acquire();


	}

	void Mouse::update(const GameTimer & gameTime)
	{
		if (mDevice != nullptr)
		{
			memcpy(&mLastState, &mCurrentState, sizeof(mCurrentState));
			if (FAILED(mDevice->GetDeviceState(sizeof(mCurrentState), &mCurrentState)))
			{
				if (SUCCEEDED(mDevice->Acquire()))
				{
					if (FAILED(mDevice->GetDeviceState(sizeof(mCurrentState), &mCurrentState)))
					{
						return;
					}
				}
			}
			mX += mCurrentState.lX;
			mY += mCurrentState.lY;
			mWheel += mCurrentState.lZ;
		}
	}

	bool Mouse::isButtonUp(MouseButtons button)const
	{
		return ((mCurrentState.rgbButtons[button]) == 0);
	}
	bool Mouse::isButtonDown(MouseButtons button)const
	{
		return ((mCurrentState.rgbButtons[button]) != 0);
	}
	bool Mouse::wasButtonUp(MouseButtons button)const
	{
		return ((mLastState.rgbButtons[button]) == 0);

	}
	bool Mouse::wasButtonDown(MouseButtons button)const
	{
		return ((mLastState.rgbButtons[button]) != 0);

	}
	bool Mouse::wasButtonPressedThisFrame(MouseButtons button)const
	{
		return isButtonDown(button) && wasButtonUp(button);
	}
	bool Mouse::wasButtonReleasedThisFrame(MouseButtons button)const
	{
		return isButtonUp(button) && wasButtonDown(button);
	}
	bool Mouse::isButtonHeldDown(MouseButtons button)const
	{
		return isButtonDown(button) && wasButtonDown(button);
	}


}
