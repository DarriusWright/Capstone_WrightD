#pragma once
#include <Window\Game.h>
#include <Lights\Light.h>


#include <Misc\Common.h>


namespace Library
{
	class PointLight : public Light
	{
		RTTI_DECLARATIONS(PointLight, Light)

	public:
		PointLight(Game& game);
		virtual ~PointLight();

		XMFLOAT3& position();
		XMVECTOR positionVector() const;
		FLOAT radius() const;

		virtual void setPosition(FLOAT x, FLOAT y, FLOAT z);
		virtual void setPosition(FXMVECTOR position);
		virtual void setPosition(const XMFLOAT3& position);
		virtual void setRadius(float value);

		static const float DefaultRadius;

	protected:
		XMFLOAT3 mPosition;
		float mRadius;
	};
}



//namespace Library
//{
//
//
//	class PointLight : public Light
//	{
//		RTTI_DECLARATIONS(PointLight, Light);
//
//	public:
//		PointLight(Game & game);
//		~PointLight();
//		void  setPosition(const XMFLOAT3 & position);
//		void  setPosition(const XMVECTOR & position);
//		const XMFLOAT3 & position()const;
//		const XMVECTOR & positionVector()const;
//		float radius()const;
//		void setRadius(const float & radius);
//	private:
//		XMFLOAT3 mPosition;
//		FLOAT mRadius;
//	};
//
//}