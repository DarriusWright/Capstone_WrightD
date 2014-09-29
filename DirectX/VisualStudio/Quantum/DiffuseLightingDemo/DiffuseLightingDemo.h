#pragma once

#include <Components\DrawableGameComponent.h>
#include <Effects\Effect.h>
#include <Materials\DiffuseLightingMaterial.h>
#include <Input\Keyboard.h>
#include <Lights\DirectionalLight.h>
#include <ProxyModel.h>

namespace Library
{


	class DiffuseLightingDemo : public DrawableGameComponent
	{
		RTTI_DECLARATIONS(DiffuseLightingDemo, DrawableGameComponent);

	public:
		DiffuseLightingDemo(Game & game, Camera & camera);
		~DiffuseLightingDemo();

		virtual void initialize()override;
		virtual void update(const GameTimer & gameTimer)override;
		virtual void draw(const GameTimer & gameTimer)override;

	private:
		DiffuseLightingDemo();
		DiffuseLightingDemo(const DiffuseLightingDemo & rhs);
		DiffuseLightingDemo & operator=(const DiffuseLightingDemo & rhs);

		void updateAmbientLight(const GameTimer & gameTimer);
		void updateDirectionalLight(const GameTimer & gameTimer);

		static const float AMBIENT_MODULATION_RATE;
		static const XMFLOAT2 ROTATION_RATE;
		RenderStateHelper * mRenderStateHelper;
		Effect * mEffect;
		DiffuseLightingMaterial * mMaterial;
		ID3D11ShaderResourceView * mTextureShaderResourceView;
		ID3D11Buffer * mVertexBuffer;
		ID3D11Buffer * mIndexBuffer;
		UINT mIndexCount;


		XMCOLOR mAmbientColor;
		DirectionalLight * mDirectionalLight;
		Keyboard * mKeyboard;
		XMFLOAT4X4 mWorldMatrix;
		ProxyModel * mProxyModel;

	};

}