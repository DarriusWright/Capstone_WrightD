#pragma once

#include <Components\DrawableGameComponent.h>
#include <Effects\Effect.h>
#include <Materials\PointLightMaterial.h>
#include <Input\Keyboard.h>
#include <Lights\DirectionalLight.h>
#include <ProxyModel.h>
#include <Lights\PointLight.h>

namespace Library
{


	class PointLightDemo : public DrawableGameComponent
	{
		RTTI_DECLARATIONS(PointLightDemo, DrawableGameComponent);

	public:
		PointLightDemo(Game & game, Camera & camera);
		~PointLightDemo();

		virtual void initialize()override;
		virtual void update(const GameTimer & gameTimer)override;
		virtual void draw(const GameTimer & gameTimer)override;

	private:
		PointLightDemo();
		PointLightDemo(const PointLightDemo & rhs);
		PointLightDemo & operator=(const PointLightDemo & rhs);

		void updateAmbientLight(const GameTimer & gameTimer);
		void updatePointLight(const GameTimer & gameTimer);

		static const float MOVEMENT_RATE;
		static const XMFLOAT2 ROTATION_RATE;

		
		Effect * mEffect;
		PointLightMaterial * mMaterial;
		ID3D11ShaderResourceView * mTextureShaderResourceView;
		ID3D11Buffer * mVertexBuffer;
		ID3D11Buffer * mIndexBuffer;
		UINT mIndexCount;


		XMCOLOR mAmbientColor;
		XMCOLOR mSpecularColor;
		PointLight * mPointLight;
		Keyboard * mKeyboard;
		XMFLOAT4X4 mWorldMatrix;
		ProxyModel * mProxyModel;
		FLOAT mSpecularPower;

	};

}