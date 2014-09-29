#pragma once
#include <Misc\Common.h>
#include <Components\DrawableGameComponent.h>
#include <Effects\Effect.h>
#include <Materials\SkyboxMaterial.h>

namespace Library
{
	class Skybox : public DrawableGameComponent
	{
		RTTI_DECLARATIONS(Skybox, DrawableGameComponent);
	public:
		Skybox(Game & game, Camera & camera , const std::wstring & cubeMapFile, float scale);
		~Skybox();
		void initialize()override;
		void update(const GameTimer & gameTimer)override;
		void draw(const GameTimer & gameTimer)override;

	private :
		Skybox();
		Skybox(const Skybox & rhs);
		Skybox & operator=(const Skybox & rhs);

		std::wstring mCubeMapFileName;
		Effect * mEffect;
		SkyboxMaterial * mMaterial;
		ID3D11ShaderResourceView * mCubeMapShaderResourceView;
		ID3D11Buffer * mVertexBuffer;
		ID3D11Buffer * mIndexBuffer;


		UINT mIndexCount;
		XMFLOAT4X4 mWorldMatrix;
		XMFLOAT4X4 mScaleMatrix;
	};

}