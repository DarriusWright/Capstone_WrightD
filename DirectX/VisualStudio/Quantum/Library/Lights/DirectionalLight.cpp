#include "DirectionalLight.h"
#include <Misc\VectorHelper.h>

namespace Library
{

	RTTI_DEFINITIONS(DirectionalLight);
	DirectionalLight::DirectionalLight(Game & game) : Light(game),mPosition(VectorHelper::ZERO), mRight(VectorHelper::RIGHT), mUp(VectorHelper::UP), mDirection(VectorHelper::FORWARD)
	{
	}


	DirectionalLight::~DirectionalLight()
	{
	}

	void DirectionalLight::applyRotation(CXMMATRIX transform)
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

	void DirectionalLight::applyRotation(const XMFLOAT4X4& transform)
	{
		XMMATRIX transformMatrix = XMLoadFloat4x4(&transform);
		applyRotation(transformMatrix);
	}

	const XMFLOAT3 & DirectionalLight::direction()const{ return mDirection; }
	const XMFLOAT3 & DirectionalLight::right()const{ return mRight; }
	 XMVECTOR  DirectionalLight::directionVector()const{ return XMLoadFloat3(&mDirection); }
	
	const XMFLOAT3 & DirectionalLight::up()const{ return mUp; }
	 XMVECTOR DirectionalLight::upVector()const{ return XMLoadFloat3(&mUp); }
	 XMVECTOR DirectionalLight::rightVector()const{ return XMLoadFloat3(&mRight); }
}