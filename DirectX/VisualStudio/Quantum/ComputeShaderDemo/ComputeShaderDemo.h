#pragma once
#include <Components\DrawableGameComponent.h>
#include "ComputeShaderMaterial.h"
#include <Effects\Pass.h>
#include <FullScreenQuad.h>
using namespace Library;

class ComputeShaderDemo : public DrawableGameComponent
{

	RTTI_DECLARATIONS(ComputeShaderDemo, DrawableGameComponent);

public:
	ComputeShaderDemo(Game & game, Camera & camera);
	~ComputeShaderDemo();

	void initialize()override;
	void update(const GameTimer & timer) override;
	void draw(const GameTimer & timer) override;
	void updateRenderingMaterial();
private:
	static const unsigned int THREADS_PER_GROUP = 32;
	ID3D11UnorderedAccessView * mOutputTexture;
	ID3D11ShaderResourceView * mColorTexture;
	ComputeShaderMaterial * mMaterial;
	Effect * mEffect;
	XMFLOAT2 mTextureSize;
	XMFLOAT2 mThreadGroupCount;
	float mBlueColor;
	Pass * mComputePass;
	FullScreenQuad * mFullScreenQuad;
};

