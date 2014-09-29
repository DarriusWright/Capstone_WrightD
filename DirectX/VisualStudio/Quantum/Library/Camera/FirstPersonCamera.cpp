#include "FirstPersonCamera.h"

namespace Library
{
	RTTI_DEFINITIONS(FirstPersonCamera);
	const float FirstPersonCamera::DFAULT_MOUSE_SENSITIVITY = 100.0f;
	const float FirstPersonCamera::DEFAULT_ROTATION_RATE = XMConvertToRadians(1.0f);
	const float FirstPersonCamera::DEFAULT_MOVEMENTRATE = 10.0f;

	FirstPersonCamera::FirstPersonCamera(Game & game) : Camera(game), mMovementRate(DEFAULT_MOVEMENTRATE), mRotationRate(DEFAULT_ROTATION_RATE), mMouseSensitivity(DFAULT_MOUSE_SENSITIVITY)
	{
	}

	FirstPersonCamera::FirstPersonCamera(Game & game, float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
		: Camera(game, fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance), mMovementRate(DEFAULT_MOVEMENTRATE), mRotationRate(DEFAULT_ROTATION_RATE), mMouseSensitivity(DFAULT_MOUSE_SENSITIVITY)
	{

	}


	FirstPersonCamera::~FirstPersonCamera()
	{
	}

	void FirstPersonCamera::initialize()
	{
		mKeyboard = (Keyboard*)mGame->getServiceContainer().getService(Keyboard::typeIdClass());
		mMouse = (Mouse*)mGame->getServiceContainer().getService(Mouse::typeIdClass());
		Camera::initialize();
	}

	void FirstPersonCamera::update(const GameTimer & gametimer)
	{
		XMFLOAT2 movementAmount = { 0.0f, 0.0f };

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
		}

		XMFLOAT2 rotationAmount = { 0, 0 };
		if ((mMouse != nullptr))
		{
			if ((mMouse->isButtonDown(MouseButtons::MouseButtonsLeft)))
			{

				LPDIMOUSESTATE mouseState = mMouse->currentState();
				rotationAmount.x = -mouseState->lX * mMouseSensitivity;
				rotationAmount.y = -mouseState->lY * mMouseSensitivity;
			}
		}

		float elapsedTime = (float)gametimer.elapsedGameTime();
		XMVECTOR rotationVector = XMLoadFloat2(&rotationAmount) * mRotationRate * elapsedTime;
		XMVECTOR right = XMLoadFloat3(&mRight);

		XMMATRIX pitchMatrix = XMMatrixRotationAxis(right, XMVectorGetY(rotationVector));
		XMMATRIX yawMatrix = XMMatrixRotationY(XMVectorGetX(rotationVector));

		applyRotation(XMMatrixMultiply(pitchMatrix, yawMatrix));

		XMVECTOR position = XMLoadFloat3(&mPosition);
		XMVECTOR movement = XMLoadFloat2(&movementAmount) * mMovementRate * elapsedTime;

		XMVECTOR strafe = right * XMVectorGetX(movement);
		position += strafe;

		XMVECTOR forward = XMLoadFloat3(&mDirection) * XMVectorGetY(movement);
		position += forward;

		XMStoreFloat3(&mPosition, position);

		Camera::update(gametimer);
	}

}