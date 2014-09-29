#pragma once

#include <Camera\RayTraceCamera.h>
#include <Input/Keyboard.h>
#include <Input/Mouse.h>
namespace Library
{

	class ThinLensCamera : public RayTraceCamera
	{
	public:
		ThinLensCamera(Game & game);
		~ThinLensCamera();
		virtual void initialize() override;
		virtual void update(const GameTimer & timer)override;

		void setViewDistance(float distance);
		void setFocalDistance(float distance);
		void setLensRadius(float radius);

		float viewDistance();
		float focalDistance();
		float lensRadius();

	protected:
		float mViewDistance;
		float mFocalDistance;
		float mLensRadius;
		float mMovementRate;
		Keyboard * mKeyboard;
		Mouse * mMouse;
	};

}