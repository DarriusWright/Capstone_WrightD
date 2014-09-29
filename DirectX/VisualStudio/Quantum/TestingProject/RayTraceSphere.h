#pragma once

#include <Components\DrawableGameComponent.h>
#include <Effects\Effect.h>
#include <Materials\Material.h>
#include <Effects\Pass.h>

using namespace Library;


class RayTraceSphere : public DrawableGameComponent
{

	RTTI_DECLARATIONS(RayTraceSphere, DrawableGameComponent);


public:
	RayTraceSphere(Game & game, Camera & camera, XMFLOAT3 position = {0,0,0}, float radius = 50.0f);
	~RayTraceSphere();
	RayTraceSphere();

	void initialize()override;
	void update(const GameTimer & timer)override;
	void draw(const GameTimer & timer)override;

	float radius()const;
	void setRadius(float radius);
	XMFLOAT3 position()const;
	XMVECTOR positionVector()const;

	void setPosition(XMFLOAT3 position);
	void setPosition(XMVECTOR position);
	void setPosition(float x, float y, float z);

	XMFLOAT3 color()const;
	XMVECTOR colorVector()const;

	void setColor(XMFLOAT3 color);
	void setColor(XMVECTOR color);
	void setColor(float x, float y, float z);


	//Effect * effect();
	//Pass * pass();
	//Material * material();


private:
	XMFLOAT3 mColor;
	XMFLOAT3 mPosition;
	float mRadius;
	//Effect * mEffect;
	//Material * mMaterial;
	//Pass * mPass;
};

