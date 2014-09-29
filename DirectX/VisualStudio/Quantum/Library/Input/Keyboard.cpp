#include "Keyboard.h"

namespace Library
{
	RTTI_DEFINITIONS(Keyboard);
	Keyboard::Keyboard(Game & game, LPDIRECTINPUT8  directInput) : GameComponent(game), mDirectInput(directInput), mDevice(nullptr)
	{
		assert(mDirectInput != nullptr);
		ZeroMemory(mCurrentState, sizeof(mCurrentState));
		ZeroMemory(mLastState, sizeof(mLastState));
	}


	Keyboard::~Keyboard()
	{
		if (mDevice != nullptr)
		{
			mDevice->Unacquire();
			mDevice->Release();
			mDevice = nullptr;
		}
	}

	const byte * const Keyboard::currentState()const
	{
		return mCurrentState;
	}

	const byte * const Keyboard::lastState()const
	{
		return mLastState;
	}

	void Keyboard::initialize()
	{
		if (FAILED(mDirectInput->CreateDevice(GUID_SysKeyboard, &mDevice, nullptr)))
		{
			throw GameException(L"IDIRECTINPUT8::CreateDevice() failed");
		}
		if (FAILED(mDevice->SetDataFormat(&c_dfDIKeyboard)))
		{
			throw GameException(L"IDIRECTINPUTDEVICE8::SetDataFormat() failed");
		}

		if (FAILED(mDevice->SetCooperativeLevel(mGame->windowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
		{
			throw GameException(L"IDIRECTINPUTDEVICE8::SetCoorperationLevel() failed");
		}

		mDevice->Acquire();
	}

	void Keyboard::update(const GameTimer & gameTime)
	{
		if (mDevice != nullptr)
		{
			memcpy(mLastState, mCurrentState, sizeof(mCurrentState));
			if (FAILED(mDevice->GetDeviceState(sizeof(mCurrentState), (LPVOID)mCurrentState)))
			{
				if (SUCCEEDED(mDevice->Acquire()))
				{
					mDevice->GetDeviceState(sizeof(mCurrentState), (LPVOID)mCurrentState);
				}
			}

		}
	}

	bool Keyboard::isKeyUp(byte key) const
	{
		return ((mCurrentState[key] & 0x80) == 0);
	}
	bool Keyboard::isKeyDown(byte key)const
	{
		return ((mCurrentState[key] & 0x80) != 0);

	}
	bool Keyboard::wasKeyUp(byte key)const
	{
		return ((mLastState[key] & 0x80) == 0);

	}
	bool Keyboard::wasKeyDown(byte key)const
	{
		return ((mLastState[key] & 0x80) != 0);

	}
	bool Keyboard::wasKeyPressedThisFrame(byte key)const
	{
		return isKeyDown(key) && wasKeyUp(key);
	}
	bool Keyboard::wasKeyReleasedThisFrame(byte key)const
	{
		return isKeyUp(key) && wasKeyDown(key);

	}
	bool Keyboard::isKeyHeldDown(byte key)const
	{
		return isKeyDown(key) && wasKeyDown(key);
	}

}