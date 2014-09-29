#pragma once
#include <Window\Game.h>
#include <Lights\Light.h>


namespace Library
{

	class DirectionalLight : public Light
	{
		RTTI_DECLARATIONS(DirectionalLight, Light);
	public:
		DirectionalLight(Game & game);
		~DirectionalLight();
		const XMFLOAT3 & direction()const;
		 XMVECTOR  directionVector()const;

		const XMFLOAT3 & up()const;
		 XMVECTOR upVector()const;
		const XMFLOAT3 & right()const;
		XMVECTOR  rightVector()const;
		void applyRotation(CXMMATRIX transform);
		void applyRotation(const XMFLOAT4X4 & transform);

	private:
		XMFLOAT3 mPosition;
		XMFLOAT3 mRight;
		XMFLOAT3 mUp;
		XMFLOAT3 mDirection;

	};

}