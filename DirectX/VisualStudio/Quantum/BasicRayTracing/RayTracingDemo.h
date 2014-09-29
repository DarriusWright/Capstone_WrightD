

#pragma once
#include <Components\DrawableGameComponent.h>
#include "RayTracerMaterial.h"
#include <Effects\Pass.h>
#include <FullScreenQuad.h>
#include <Input\Keyboard.h>
using namespace Library;

class RayTracingDemo : public DrawableGameComponent
{

	RTTI_DECLARATIONS(RayTracingDemo, DrawableGameComponent);

public:
	RayTracingDemo(Game & game, Camera & camera);
	~RayTracingDemo();

	void initialize()override;
	void update(const GameTimer & timer) override;
	void draw(const GameTimer & timer) override;
	void updateRenderingMaterial();
private:
	static const unsigned int THREADS_PER_GROUP = 32;
	ID3D11UnorderedAccessView * mOutputTexture;
	ID3D11ShaderResourceView * mColorTexture;
	RayTracerMaterial * mMaterial;
	Effect * mEffect;
	XMFLOAT2 mTextureSize;
	XMFLOAT2 mThreadGroupCount;
	float mBlueColor;
	Pass * mComputePass;
	FullScreenQuad * mFullScreenQuad;
	XMFLOAT3 mPosition;
	XMFLOAT3 mLightPosition;
	Keyboard * mKeyboard;
};

