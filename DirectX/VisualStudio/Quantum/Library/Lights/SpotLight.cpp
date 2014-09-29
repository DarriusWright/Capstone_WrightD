#include "SpotLight.h"
#include <Misc\VectorHelper.h>
namespace Library
{
	RTTI_DEFINITIONS(SpotLight);
	const float SpotLight::DEFAULT_INNER_ANGLE = 0.75f;
	const float SpotLight::DEFAULT_OUTER_ANGLE = 0.25f;

	SpotLight::SpotLight(Game & game) : PointLight(game), mInnerAngle(DEFAULT_INNER_ANGLE), mOuterAngle(DEFAULT_OUTER_ANGLE),
		mRight(VectorHelper::RIGHT), mUp(VectorHelper::UP), mDirection(VectorHelper::FORWARD)
	{
	}


	SpotLight::~SpotLight()
	{
	}


	

	void SpotLight::applyRotation(CXMMATRIX transform)
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

	void SpotLight::applyRotation(const XMFLOAT4X4& transform)
	{
		XMMATRIX transformMatrix = XMLoadFloat4x4(&transform);
		applyRotation(transformMatrix);
	}

	const XMFLOAT3 & SpotLight::direction()const{ return mDirection; }
	const XMFLOAT3 & SpotLight::right()const{ return mRight; }
	XMVECTOR  SpotLight::directionVector()const{ return XMLoadFloat3(&mDirection); }

	const XMFLOAT3 & SpotLight::up()const{ return mUp; }
	XMVECTOR SpotLight::upVector()const{ return XMLoadFloat3(&mUp); }
	XMVECTOR SpotLight::rightVector()const{ return XMLoadFloat3(&mRight); }

	float SpotLight::innerAngle(){ return mInnerAngle; }
	void SpotLight::setInnerAngle(float angle) { mInnerAngle = angle; }

	float SpotLight::outerAngle() { return mOuterAngle; }
	void SpotLight::setOuterAngle(float angle) { mOuterAngle = angle; }
}