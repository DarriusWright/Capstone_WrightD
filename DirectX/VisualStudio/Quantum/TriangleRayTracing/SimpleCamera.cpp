#include "SimpleCamera.h"


SimpleCamera::SimpleCamera(Game & game , float viewPlaneDistance) : mUp(0, 1, 0), mViewPlaneDistance(viewPlaneDistance)
, mGame(&game), mMovementRate(10.0f), mW(0, 0, 1), mV(0, 1, 0), mU(1,0,0)
{
}


SimpleCamera::~SimpleCamera()
{
}

void SimpleCamera::computeUVW()
{
	//camera direction 
	XMStoreFloat3(&mW, -(eyeVector() - XMLoadFloat3(&mLookAt)));
	XMStoreFloat3(&mW, XMVector3Normalize(XMLoadFloat3(&mW)));

	//up crossed with camera direction
	XMStoreFloat3(&mU, XMVector3Cross(XMLoadFloat3(&mW), XMLoadFloat3(&mUp)));
	XMStoreFloat3(&mU, XMVector3Normalize(XMLoadFloat3(&mU)));

	XMStoreFloat3(&mV, XMVector3Cross(XMLoadFloat3(&mU), XMLoadFloat3(&mW)));
}


void SimpleCamera::initialize()
{
	//Camera::initialize();
	mKeyboard = (Keyboard*)mGame->getServiceContainer().getService(Keyboard::typeIdClass());
	mMouse = (Mouse*)mGame->getServiceContainer().getService(Mouse::typeIdClass());
	computeUVW();
}

void SimpleCamera::update(const GameTimer & timer)
{
	//RayTraceCamera::update(timer);
	XMFLOAT3 movementAmount = { 0.0f, 0.0f, 0.0f };

	if (mKeyboard != nullptr)
	{
		if (mKeyboard->isKeyDown(DIK_W))
		{
			movementAmount.y = 1.0f;
		}
		if (mKeyboard->isKeyDown(DIK_S))
		{
			movementAmount.y = -1.0f;
		}
		if (mKeyboard->isKeyDown(DIK_A))
		{
			movementAmount.x = -1.0f;
		}
		if (mKeyboard->isKeyDown(DIK_D))
		{
			movementAmount.x = 1.0f;
		}


		if (mKeyboard->isKeyDown(DIK_R))
		{
			movementAmount.z = -1.0f;
		}
		if (mKeyboard->isKeyDown(DIK_F))
		{
			movementAmount.z = 1.0f;
		}

		if (mKeyboard->isKeyDown(DIK_Q))
		{
			mViewPlaneDistance -= 10.0f * timer.elapsedGameTime();
		}
		if (mKeyboard->isKeyDown(DIK_E))
		{
			mViewPlaneDistance += 10.0f * timer.elapsedGameTime();
		}
	}

	float rotationAmount = 0;
	if ((mMouse != nullptr))
	{
		if ((mMouse->isButtonDown(MouseButtons::MouseButtonsLeft)))
		{

			LPDIMOUSESTATE mouseState = mMouse->currentState();
			rotationAmount = -mouseState->lX;
			//rotationAmount.y = -mouseState->lY * mMouseSensitivity;
		}
	}
	//XMStoreFloat3(&mUp,XMVector3Transform(upVector(), XMMatrixRotationZ(rotationAmount/180.0f * 3.14)));
	XMVECTOR position = eyeVector();
	XMVECTOR movement = XMLoadFloat3(&movementAmount) * mMovementRate * timer.elapsedGameTime();

	/*XMVECTOR strafe = right * XMVectorGetX(movement);
	position += strafe;

	XMVECTOR forward = XMLoadFloat3(&mDirection) * XMVectorGetY(movement);
	position += forward;*/

	XMStoreFloat3(&mPosition, position + movement);
	computeUVW();
}

XMFLOAT3 SimpleCamera::eye(){ return mPosition; }
XMVECTOR SimpleCamera::eyeVector(){ return XMLoadFloat3(&mPosition); }
void SimpleCamera::setEyeVector(float x, float y, float z) { mPosition = { x, y, z }; }
void SimpleCamera::setEyeVector(XMFLOAT3 eye){ mPosition = eye; }
void SimpleCamera::setEyeVector(XMVECTOR eye){ XMStoreFloat3(&mPosition, eye); }

XMFLOAT3 SimpleCamera::u()const{ return mU; }
XMFLOAT3 SimpleCamera::v()const{ return mV; }
XMFLOAT3 SimpleCamera::w()const{ return mW; }
XMFLOAT3 SimpleCamera::lookAt()const{ return mLookAt; }

XMVECTOR SimpleCamera::uVector()
{ 
	return XMLoadFloat3(&mU); 
}
XMVECTOR SimpleCamera::vVector()
{
	return XMLoadFloat3(&mV); 
}
XMVECTOR SimpleCamera::wVector()
{ 
	return XMLoadFloat3(&mW); 
}
XMVECTOR SimpleCamera::lookAtVector(){ return XMLoadFloat3(&mLookAt); }

void SimpleCamera::setLookAt(float x, float y, float z){ mLookAt = XMFLOAT3(x, y, z); }
void SimpleCamera::setLookAt(XMFLOAT3 lookAtPoint){ mLookAt = lookAtPoint; }
void SimpleCamera::setLookAt(XMVECTOR lookAtPoint){ XMStoreFloat3(&mLookAt, lookAtPoint); }

void SimpleCamera::setViewDistance(float distance)
{
	mViewPlaneDistance = distance;
}
float SimpleCamera::viewDistance()
{
	return mViewPlaneDistance;
}
