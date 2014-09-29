#pragma once
#include <Window\Game.h>
#include <Misc\Common.h>
#include <Camera/Camera.h>
#include <Input/Keyboard.h>
#include <Input/Mouse.h>
using namespace Library;

class SimpleCamera
{
public:
	SimpleCamera(Game & game, float viewPlaneDistance = 1000);
	~SimpleCamera();

	XMFLOAT3 u()const;
	XMFLOAT3 v()const;
	XMFLOAT3 w()const;
	XMFLOAT3 lookAt()const;

	XMVECTOR uVector();
	XMVECTOR vVector();
	XMVECTOR wVector();
	XMVECTOR lookAtVector();

	void setLookAt(float x, float y, float z);
	void setLookAt(XMFLOAT3 lookAtPoint);
	void setLookAt(XMVECTOR lookAtPoint);

	void computeUVW();

	XMFLOAT3 eye();
	XMVECTOR eyeVector();
	void setEyeVector(float x, float y, float z);
	void setEyeVector(XMFLOAT3 eye);
	void setEyeVector(XMVECTOR eye);
	void update(const GameTimer & timer);
	void initialize();
	void setViewDistance(float distance);
	float viewDistance();
	//void update(const GameTimer & timer)override;

protected:
	Game * mGame;
	float mViewPlaneDistance;
	XMFLOAT3 mLookAt;
	XMFLOAT3 mU;
	XMFLOAT3 mV;
	XMFLOAT3 mW;
	XMFLOAT3 mUp;
	XMFLOAT3 mPosition;
	//float mViewPlaneDistance;
	float mZoom;
	float mMovementRate;
	float mRollAngle;
	Keyboard *mKeyboard;
	Mouse * mMouse;
	float mMouseSensitivity;
};

