#pragma once

#pragma once
#include <Misc\Common.h>
#include <Camera/Camera.h>
#include <Input/Keyboard.h>
#include <Input/Mouse.h>
#include <Camera\RayTraceCamera.h>


namespace Library
{
	//Any camera in ray tracing must have an orthonormal basis (u,v,w)
	/*
	this is defined by three vectors the...
	viewDirection = normalized(eyePosition -lookAt);
	w = viewDirection;
	upVector = (0,1,0);
	u = normalize(upVector x viewDirection)

	v = w x u
	*/

	class PinholeCamera : public RayTraceCamera
	{
		RTTI_DECLARATIONS(PinholeCamera, RayTraceCamera);

	public:
		PinholeCamera(Game & game, float viewPlaneDistance = 1000);
		~PinholeCamera();

		void setViewDistance(float distance);
		float viewDistance();
		void initialize()override;
		void update(const GameTimer & timer)override;
		void setRollAngle(float angle);
		float rollAngle();
	protected:
		float mViewPlaneDistance;
		float mZoom;
		float mMovementRate;
		float mRollAngle;
		Keyboard *mKeyboard;
		Mouse * mMouse;
		float mMouseSensitivity;
	};

}