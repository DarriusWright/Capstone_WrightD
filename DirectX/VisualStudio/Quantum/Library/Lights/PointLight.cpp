
#include "PointLight.h"
#include <Misc\VectorHelper.h>

namespace Library
{
	RTTI_DEFINITIONS(PointLight)

		const float PointLight::DefaultRadius = 10.0f;

	PointLight::PointLight(Game& game)
		: Light(game), mPosition(VectorHelper::ZERO), mRadius(DefaultRadius)
	{
	}

	PointLight::~PointLight()
	{
	}

	XMFLOAT3& PointLight::position()
	{
		return mPosition;
	}

	XMVECTOR PointLight::positionVector() const
	{
		return XMLoadFloat3(&mPosition);
	}

	float PointLight::radius() const
	{
		return mRadius;
	}

	void PointLight::setPosition(FLOAT x, FLOAT y, FLOAT z)
	{
		XMVECTOR position = XMVectorSet(x, y, z, 1.0f);
		setPosition(position);
	}

	void PointLight::setPosition(FXMVECTOR position)
	{
		XMStoreFloat3(&mPosition, position);
	}

	void PointLight::setPosition(const XMFLOAT3& position)
	{
		mPosition = position;
	}

	void PointLight::setRadius(float value)
	{
		mRadius = value;
	}
}

//#include "PointLight.h"
//
//namespace Library
//{
//
//	RTTI_DEFINITIONS(PointLight);
//
//	PointLight::PointLight(Game & game) : Light(game), mPosition(0, 0, 0), mRadius(10.0f)
//	{
//	}
//
//
//	PointLight::~PointLight()
//	{
//	}
//
//	void  PointLight::setPosition(const XMFLOAT3 & position){ mPosition = position; }
//	void  PointLight::setPosition(const XMVECTOR & position){ XMStoreFloat3(&mPosition, position); }
//	const XMFLOAT3 & PointLight::position()const{ return mPosition; }
//	const XMVECTOR & PointLight::positionVector()const{ return XMLoadFloat3(&mPosition); }
//	float  PointLight::radius()const{ return mRadius; }
//	void PointLight::setRadius(const FLOAT & radius){ mRadius = radius; }
//}