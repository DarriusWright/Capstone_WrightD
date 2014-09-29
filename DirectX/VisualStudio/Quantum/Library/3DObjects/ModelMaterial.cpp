#include "ModelMaterial.h"
#include <Exceptions\GameException.h>
#include <Misc\Utility.h>
#include <assimp\scene.h>

namespace Library
{
	std::map<TextureType, UINT> ModelMaterial::sTextureTypeMappings;

	ModelMaterial::ModelMaterial(Model & model) : mModel(model)
	{
		initializeTextureTypeMappings();

	}

	ModelMaterial::ModelMaterial(Model & model, aiMaterial * material) : mModel(model), mAiMaterial(material)
	{
		initializeTextureTypeMappings();
		aiString name;
		material->Get(AI_MATKEY_NAME, name);
		mName = name.C_Str();

		for (TextureType textureType = (TextureType)0; textureType < TextureType::End;textureType = (TextureType)(textureType + 1))
		{
			aiTextureType mappedTextureType = (aiTextureType)sTextureTypeMappings[textureType];
			UINT textureCount = material->GetTextureCount(mappedTextureType);
			if (textureCount > 0)
			{
				std::vector<std::wstring>* textures = new std::vector<std::wstring>();
				mTextures.insert(std::pair<TextureType, std::vector<std::wstring>*>(textureType, textures));
				textures->reserve(textureCount);
				for (UINT textureIndex = 0; textureIndex < textureCount; textureIndex++)
				{
					aiString path;
					if (material->GetTexture(mappedTextureType, textureIndex, &path) == AI_SUCCESS)
					{
						std::wstring wPath;
						Utility::ToWideString(path.C_Str(),wPath);
						textures->push_back(wPath);
					}
				}
			}
		}
	}

	ModelMaterial::~ModelMaterial()
	{
		for (std::pair<TextureType,std::vector<std::wstring>*>textures : mTextures)
		{
			DeleteObject(textures.second);
		}
	}
	const std::string & ModelMaterial::name() const
	{
		return mName;
	}
	const std::map<TextureType, std::vector<std::wstring>*> ModelMaterial::textures()const
	{
		return mTextures;
	}

	void ModelMaterial::initializeTextureTypeMappings()
	{
		if (sTextureTypeMappings.size() != TextureType::End)
		{
			sTextureTypeMappings[TextureType::Diffuse] = aiTextureType_DIFFUSE;
			sTextureTypeMappings[TextureType::SpecularMap] = aiTextureType_SPECULAR;
			sTextureTypeMappings[TextureType::Ambient] = aiTextureType_AMBIENT;
			sTextureTypeMappings[TextureType::Emissive] = aiTextureType_EMISSIVE;
			sTextureTypeMappings[TextureType::Heightmap] = aiTextureType_HEIGHT;
			sTextureTypeMappings[TextureType::NormalMap] = aiTextureType_NORMALS;
			sTextureTypeMappings[TextureType::SpecularPowerMap] = aiTextureType_SHININESS;
			sTextureTypeMappings[TextureType::DisplacementMap] = aiTextureType_DISPLACEMENT;
			sTextureTypeMappings[TextureType::LightMap] = aiTextureType_LIGHTMAP;

		}
	}

}