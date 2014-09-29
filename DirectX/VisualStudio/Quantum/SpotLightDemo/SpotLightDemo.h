#pragma once


#include <Components\DrawableGameComponent.h>


using namespace Library;

namespace Library
{
	class Effect;
	class SpotLightMaterial;
	class SpotLight;
	class Keyboard;
	class Mouse;
	class ProxyModel;
}

class SpotLightDemo : public DrawableGameComponent
{

	RTTI_DECLARATIONS(SpotLightDemo, DrawableGameComponent);

public:
	SpotLightDemo(Game & game, Camera & camera);
	~SpotLightDemo();
	virtual void initialize()override;
	virtual void update(const GameTimer & gameTimer)override;
	virtual void draw(const GameTimer & gameTimer)override;

private:
	SpotLightDemo();
	SpotLightDemo(const SpotLightDemo & rhs);
	SpotLightDemo & operator=(const SpotLightDemo & rhs);

	void updateAmbientLight(const GameTimer & gameTimer);
	void updateSpecular(const GameTimer & gameTimer);
	void updateSpotLight(const GameTimer & gameTimer);

	static const float MOVEMENT_RATE;
	static const XMFLOAT2 ROTATION_RATE;


	Effect * mEffect;
	SpotLightMaterial * mMaterial;
	ID3D11ShaderResourceView * mTextureShaderResourceView;
	ID3D11Buffer * mVertexBuffer;
	ID3D11Buffer * mIndexBuffer;
	UINT mIndexCount;


	XMCOLOR mAmbientColor;
	XMCOLOR mSpecularColor;
	SpotLight * mSpotLight;
	Keyboard * mKeyboard;
	XMFLOAT4X4 mWorldMatrix;
	ProxyModel * mProxyModel;
	FLOAT mSpecularPower;


};

