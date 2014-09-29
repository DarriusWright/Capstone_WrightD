#pragma once

#include <functional>
#include <Components\DrawableGameComponent.h>
#include <Vertex\VertexPositionTexture.h>


namespace Library
{
	class Effect;
	class Material;
	class Pass;

	class FullScreenQuad : public DrawableGameComponent
	{
		RTTI_DECLARATIONS(FullScreenQuad, DrawableGameComponent)

	public:
		FullScreenQuad(Game& game);
		FullScreenQuad(Game& game, Material& material);
		~FullScreenQuad();

		Material* GetMaterial();
		void SetMaterial(Material& material, const std::string& techniqueName, const std::string& passName);
		void SetActiveTechnique(const std::string& techniqueName, const std::string& passName);
		void SetCustomUpdateMaterial(std::function<void()> callback);

		virtual void initialize() override;
		virtual void draw(const GameTimer & gameTime) override;

	private:
		FullScreenQuad();
		FullScreenQuad(const FullScreenQuad& rhs);
		FullScreenQuad& operator=(const FullScreenQuad& rhs);

		Material* mMaterial;
		Pass* mPass;
		ID3D11InputLayout* mInputLayout;

		ID3D11Buffer* mVertexBuffer;
		ID3D11Buffer* mIndexBuffer;
		UINT mVertexCount;
		UINT mIndexCount;
		std::function<void()> mCustomUpdateMaterial;
	};
}