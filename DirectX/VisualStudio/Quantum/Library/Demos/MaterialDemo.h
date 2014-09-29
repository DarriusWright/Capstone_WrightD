#pragma once
#include <Components\DrawableGameComponent.h>

using namespace Library;



namespace Library
{
	class Effect;
	class BasicMaterial;
}

class MaterialDemo : public DrawableGameComponent
{
	RTTI_DECLARATIONS(MaterialDemo, DrawableGameComponent);
public:
	MaterialDemo(Game & game, Camera & camera);
	~MaterialDemo();

	virtual void initialize() override;
	virtual void draw(const GameTimer & gameTIme);

private:
	MaterialDemo();
	MaterialDemo(const MaterialDemo & rhs);
	MaterialDemo & operator=(const MaterialDemo & rhs);
	
	Effect * mBasicEffect;
	BasicMaterial * mBasicMaterial;
	ID3D11Buffer * mVertexBuffer;
	ID3D11Buffer * mIndexBuffer;
	UINT mIndexCount;

	XMFLOAT4X4 mWorldMatrix;

};

