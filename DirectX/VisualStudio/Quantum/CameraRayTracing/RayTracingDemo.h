

#pragma once
#include <Components\DrawableGameComponent.h>
#include "RayTracerMaterial.h"
#include <Effects\Pass.h>
#include <FullScreenQuad.h>
#include <Input\Keyboard.h>
#include "RayTraceSphere.h"
#include <Camera\PinholeCamera.h>
using namespace Library;

class RayTracingDemo : public DrawableGameComponent
{

	RTTI_DECLARATIONS(RayTracingDemo, DrawableGameComponent);

public:
	RayTracingDemo(Game & game, PinholeCamera & camera);
	~RayTracingDemo();

	void initialize()override;
	void update(const GameTimer & timer) override;
	void draw(const GameTimer & timer) override;
	void updateRenderingMaterial();
private:
	inline float getRandomFloat(float minRange, float maxRange)
	{
		return minRange + (float)rand() / ((float)RAND_MAX / (maxRange - minRange));
	}
	static const int NUM_SPHERE = 100;

	static float const MAX_POSITION;
	static float const MIN_POSITION;
	static const unsigned int THREADS_PER_GROUP;
	static const float CLEAR_COLOR[4];
	static const float CLEAR_DEPTH[4];
	RayTraceSphere spheres[NUM_SPHERE];
	ID3D11UnorderedAccessView * mOutputTexture;
	ID3D11UnorderedAccessView * mDepthTexture;
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
	PinholeCamera * mCamera;
};

