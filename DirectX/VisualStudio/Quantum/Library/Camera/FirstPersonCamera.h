#pragma once
#include <Misc\Common.h>
#include <Camera/Camera.h>
#include <Input/Keyboard.h>
#include <Input/Mouse.h>


namespace Library
{

	class Keyboard;
	class Mouse;
	class FirstPersonCamera : public Camera
	{
		RTTI_DECLARATIONS(FirstPersonCamera, Camera);

	public:
		FirstPersonCamera(Game & game);
		FirstPersonCamera(Game & game , float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance);
		virtual ~FirstPersonCamera();
		const Keyboard & getKeyboard()const;
		void setKeyboard(Keyboard & keyboard);
		const Mouse & getMouse()const;
		void setMouse(Mouse & mouse);

		float & mouseSensitivity();
		float & rotationRate();
		float & movementRate();

		void initialize()override;
		void update(const GameTimer & gametime) override;

		static const float DFAULT_MOUSE_SENSITIVITY;
		static const float DEFAULT_ROTATION_RATE;
		static const float DEFAULT_MOVEMENTRATE;

	protected:
		FirstPersonCamera(const FirstPersonCamera & rhs);
		FirstPersonCamera & operator=(const FirstPersonCamera & rhs);
		Keyboard * mKeyboard;
		Mouse * mMouse;
		float mMovementRate;
		float mRotationRate;
		float mMouseSensitivity;


	};
}
