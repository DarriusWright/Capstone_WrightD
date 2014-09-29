#pragma once



#include <Lights\PointLight.h>
namespace Library
{

	class SpotLight : public PointLight
	{
		RTTI_DECLARATIONS(SpotLight, PointLight);

	public:
		SpotLight(Game & game);
		~SpotLight();

		const XMFLOAT3 & direction()const;
		XMVECTOR  directionVector()const;

		const XMFLOAT3 & up()const;
		XMVECTOR upVector()const;
		const XMFLOAT3 & right()const;
		XMVECTOR  rightVector()const;
		void applyRotation(CXMMATRIX transform);
		void applyRotation(const XMFLOAT4X4 & transform);


		float innerAngle();
		void setInnerAngle(float angle);

		float outerAngle();
		void setOuterAngle(float angle);

		static const float DEFAULT_INNER_ANGLE;
		static const float DEFAULT_OUTER_ANGLE;

	private:

		XMFLOAT3 mRight;
		XMFLOAT3 mUp;
		XMFLOAT3 mDirection;

		float mInnerAngle;
		float mOuterAngle;



	};

}