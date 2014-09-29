#include "RayTraceSphere.h"


RTTI_DEFINITIONS(RayTraceSphere, DrawableGameComponent);

RayTraceSphere::RayTraceSphere(Game & game, Camera & camera, XMFLOAT3 position, float radius) :
DrawableGameComponent(game, camera),  mPosition(position), mRadius(radius)
{
}
RayTraceSphere::RayTraceSphere()
{
}

RayTraceSphere::~RayTraceSphere()
{
}



void RayTraceSphere::initialize(){}
void RayTraceSphere::update(const GameTimer & timer){}
void RayTraceSphere::draw(const GameTimer & timer){}

float RayTraceSphere::radius()const{ return mRadius; }
void RayTraceSphere::setRadius(float radius){ mRadius = radius; }
XMFLOAT3 RayTraceSphere::position()const{ return mPosition; }
XMVECTOR RayTraceSphere::positionVector()const{ return XMLoadFloat3(&mPosition); }

void RayTraceSphere::setPosition(XMFLOAT3 position){ mPosition = position; }
void RayTraceSphere::setPosition(XMVECTOR position){ XMStoreFloat3(&mPosition, position); }
void RayTraceSphere::setPosition(float x, float y, float z){ mPosition = XMFLOAT3(x, y, z); }

XMFLOAT3 RayTraceSphere::color()const{ return mColor; }
XMVECTOR RayTraceSphere::colorVector()const{ return XMLoadFloat3(&mColor); }

void RayTraceSphere::setColor(XMFLOAT3 color){ mColor = color; }
void RayTraceSphere::setColor(XMVECTOR color){ XMStoreFloat3(&mColor, color); }
void RayTraceSphere::setColor(float x, float y, float z){ mColor = XMFLOAT3(x, y, z); }


//Effect * RayTraceSphere::effect(){ return mEffect; }
//Pass * RayTraceSphere::pass(){ return mPass; }
//Material * RayTraceSphere::material(){ return mMaterial; }