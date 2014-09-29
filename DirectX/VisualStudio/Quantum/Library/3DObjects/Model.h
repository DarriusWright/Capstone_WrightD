#pragma once
#include <Misc/Common.h>

namespace Library
{
	class Game;
	class Mesh;
	class ModelMaterial;
	class Model
	{
	public:
		Model(Game & game, const std::string & fileName, ID3DX11Effect * effect, const wchar_t * texturePath = nullptr, bool flipUvs = false);
		Model(Game & game, const std::string & fileName, bool flipUvs = false);
		~Model();

		Game& getGame();
		bool hasMeshes()const;
		bool hasMaterials()const;

		const std::vector<Mesh*>& meshes()const;
		const std::vector<ModelMaterial*>& materials()const;

		const XMFLOAT3 & position();
		const XMFLOAT3 & scale();
		const XMFLOAT3 & rotation();		
		void setPosition(const XMFLOAT3 & position);
		void setScale(const XMFLOAT3 & scale);
		void setRotation(const XMFLOAT3 & rotation);

		ID3D11ShaderResourceView * textureShaderResourceView();
		ID3DX11EffectShaderResourceVariable * colorTextureVarialble();

	private:
		ID3D11ShaderResourceView * mTextureShaderResourceView;
		ID3DX11EffectShaderResourceVariable * mColorTextureVarialble;

		Model(const Model & rhs);
		Model & operator=(const Model & rhs);
		Game & mGame;
		std::vector<Mesh*>mMeshes;
		std::vector<ModelMaterial*>mMaterials;
		XMFLOAT3 mPosition;
		XMFLOAT3 mScale;
		XMFLOAT3 mRotation;
	};

}
