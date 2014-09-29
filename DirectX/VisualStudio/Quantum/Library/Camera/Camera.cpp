#include "Camera.h"

namespace Library
{

	RTTI_DEFINITIONS(Camera);

	const float Camera::DEFAULT_FIELD_OF_VIEW(XM_PIDIV4);

	const float Camera::DEFAULT_NEAR_PLANE_DISTANCE(0.01f);
	const float Camera::DEFAULT_FAR_PLANE_DISTANCE(1000.0f);

	Camera::Camera(Game & game) : GameComponent(game),
		mFieldOfView(DEFAULT_FIELD_OF_VIEW),
		mNearPlaneDistance(DEFAULT_NEAR_PLANE_DISTANCE),
		mFarPlaneDistance(DEFAULT_FAR_PLANE_DISTANCE),
		mAspectRatio(game.aspectRatio()),
		mPosition(), mDirection(), mUp(), mRight(), mViewMatrix(), mProjectionMatrix()
	{
		
	}
	Camera::Camera(Game & game, float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance) :
		GameComponent(game),
		mFieldOfView(fieldOfView),
		mNearPlaneDistance(nearPlaneDistance),
		mFarPlaneDistance(farPlaneDistance),
		mAspectRatio(aspectRatio)
	{

	}

	Camera::~Camera()
	{
	}

	XMMATRIX Camera::viewProjectionMatrix() const
	{
		XMMATRIX viewMatrix = XMLoadFloat4x4(&mViewMatrix);
		XMMATRIX projectionMatrix = XMLoadFloat4x4(&mProjectionMatrix);
		return XMMatrixMultiply(viewMatrix,projectionMatrix);
	}

	const XMFLOAT3&  Camera::position()const{ return mPosition; }
	const XMFLOAT3&  Camera::direction()const{ return mDirection; }
	const XMFLOAT3&  Camera::up()const{ return mUp; }
	const XMFLOAT3&  Camera::right(){ return mRight; }

	XMVECTOR Camera::positionVector()const
	{
		XMVECTOR positionVec = XMLoadFloat3(&mPosition);
		return positionVec;
	}
	XMVECTOR Camera::directionVector()const
	{
		XMVECTOR directionVec = XMLoadFloat3(&mDirection);
		return directionVec;
	}
	XMVECTOR Camera::upVector()const
	{
		XMVECTOR upVec = XMLoadFloat3(&mUp);
		return upVec;
	}
	XMVECTOR Camera::rightVector()const
	{
		XMVECTOR rightVec = XMLoadFloat3(&mRight);
		return rightVec;
	}

	float Camera::aspectRatio()const{ return mAspectRatio; }
	float Camera::fieldOfView()const{ return mFieldOfView; }
	float Camera::nearPlaneDistance()const{ return mNearPlaneDistance; }
	float Camera::farPlaneDistance()const{ return mFarPlaneDistance; }

	XMMATRIX  Camera::viewMatrix()const
	{
		XMMATRIX viewMat = XMLoadFloat4x4(&mViewMatrix);
		return viewMat;
	}
	XMMATRIX  Camera::projectionMatrix()const
	{
		XMMATRIX projectionMat = XMLoadFloat4x4(&mProjectionMatrix);
		return projectionMat;
	}

	void Camera::setPosition(FLOAT x, FLOAT y, FLOAT z)
	{
		mPosition = { x, y, z };
	} 
	// does FLOAT change depending on the platform
	void Camera::setPosition(FXMVECTOR position)
	{
		XMStoreFloat3(&mPosition, position);
	}
	void Camera::setPosition(const XMFLOAT3 & position)
	{
		mPosition = position;
	}
	void Camera::reset()
	{
		mPosition = { 0, 0, 0 };
		mDirection = { 0, 0, -1 };
		mUp = { 0, 1, 0 };
		mRight = { 1, 0, 0 };

		updateViewMatrix();
	}
	void Camera::initialize()
	{
		updateProjectionMatrix();
		reset();
	}
	void Camera::update(const GameTimer  & gameTime)
	{
		updateViewMatrix();

	}
	void Camera::updateViewMatrix()
	{
		XMVECTOR eyePosition = XMLoadFloat3(&mPosition);
		XMVECTOR direction = XMLoadFloat3(&mDirection);
		XMVECTOR upDirection = XMLoadFloat3(&mUp);

		XMMATRIX viewMatrix = XMMatrixLookToRH(eyePosition, direction, upDirection);
		XMStoreFloat4x4(&mViewMatrix, viewMatrix);


	}
	void Camera::updateProjectionMatrix()
	{
		XMMATRIX projectionMatrix = XMMatrixPerspectiveFovRH(mFieldOfView,mAspectRatio,mNearPlaneDistance,mFarPlaneDistance);
		XMStoreFloat4x4(&mProjectionMatrix, projectionMatrix);
	}
	void Camera::applyRotation(CXMMATRIX transform)
	{
		XMVECTOR direction = XMLoadFloat3(&mDirection);
		XMVECTOR up = XMLoadFloat3(&mUp);
		direction = XMVector3TransformNormal(direction, transform);
		direction = XMVector3Normalize(direction);

		up = XMVector3TransformNormal(up, transform);
		up = XMVector3Normalize(up);
		
		XMVECTOR right = XMVector3Cross(direction, up);
		up = XMVector3Cross(right, direction);

		XMStoreFloat3(&mDirection, direction);
		XMStoreFloat3(&mUp, up);
		XMStoreFloat3(&mRight, right);

	}
	void Camera::applyRotation(const XMFLOAT4X4 & transform)
	{
		XMMATRIX transformMatrix = XMLoadFloat4x4(&transform);
		applyRotation(transformMatrix);
	}

}