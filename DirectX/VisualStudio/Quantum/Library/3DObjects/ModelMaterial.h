#pragma once
#include <Misc\Common.h>
struct aiMaterial;

namespace Library
{
	enum TextureType
	{
		Diffuse = 0,
		SpecularMap,
		Ambient,
		Emissive,
		Heightmap,
		NormalMap,
		SpecularPowerMap,
		DisplacementMap,
		LightMap,
		End

	};
	class ModelMaterial
	{
		friend class Model;
	public:
		ModelMaterial(Model & model);
		~ModelMaterial();
		const std::string & name() const;
		const std::map<TextureType, std::vector<std::wstring>*> textures()const;
		ModelMaterial(Model & model, aiMaterial * material);

	private:
		static void initializeTextureTypeMappings();
		static std::map<TextureType, UINT> sTextureTypeMappings;
		ModelMaterial(const ModelMaterial & rhs);
		ModelMaterial& operator=(const ModelMaterial & rhs);

		Model & mModel;
		aiMaterial * mAiMaterial;
		std::string mName;
		std::map<TextureType, std::vector<std::wstring>*> mTextures;

	};
}
