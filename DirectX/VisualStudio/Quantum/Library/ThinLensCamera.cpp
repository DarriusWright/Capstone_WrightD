#include "ThinLensCamera.h"

namespace Library
{


	ThinLensCamera::ThinLensCamera(Game & game) : RayTraceCamera(game), mViewDistance(50.0f), mFocalDistance(75.0f), mMovementRate(100.0f), mLensRadius(1.0f)
		,mKeyboard(nullptr), mMouse(nullptr)
	{
	}


	ThinLensCamera::~ThinLensCamera()
	{
	}

	void ThinLensCamera::initialize()
	{
		RayTraceCamera::initialize();
		mKeyboard = (Keyboard*)mGame->getServiceContainer().getService(Keyboard::typeIdClass());
		mMouse = (Mouse*)mGame->getServiceContainer().getService(Mouse::typeIdClass());
	}
	void ThinLensCamera::update(const GameTimer & timer)
	{
		RayTraceCamera::update(timer);
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
				mViewDistance -= 10.0f * timer.elapsedGameTime();
			}
			if (mKeyboard->isKeyDown(DIK_E))
			{
				mViewDistance += 10.0f * timer.elapsedGameTime();
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
		XMStoreFloat3(&mUp, XMVector3Transform(upVector(), XMMatrixRotationZ(rotationAmount / 180.0f * 3.14)));
		XMVECTOR position = eyeVector();
		XMVECTOR movement = XMLoadFloat3(&movementAmount) * mMovementRate * timer.elapsedGameTime();
		XMStoreFloat3(&mPosition, position + movement);
	}

	void ThinLensCamera::setViewDistance(float distance){	mViewDistance = distance;}
	void ThinLensCamera::setFocalDistance(float distance){ mFocalDistance = distance; }
	void ThinLensCamera::setLensRadius(float radius){ mLensRadius = radius; }

	float ThinLensCamera::viewDistance(){ return mViewDistance; }
	float ThinLensCamera::focalDistance(){ return mFocalDistance; }
	float ThinLensCamera::lensRadius(){ return mLensRadius; }


}