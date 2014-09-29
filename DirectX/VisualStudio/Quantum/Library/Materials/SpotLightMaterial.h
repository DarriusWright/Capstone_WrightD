#pragma once

#include <Materials\Material.h>

namespace Library
{

	typedef struct _SpotLightingMaterialVertex
	{
		XMFLOAT4 position;
		XMFLOAT2 textureCoordinates;
		XMFLOAT3 normal;

		_SpotLightingMaterialVertex(){}
		_SpotLightingMaterialVertex(XMFLOAT4 position,
			XMFLOAT2 textureCoordinates,
			XMFLOAT3 normal) : position(position), textureCoordinates(textureCoordinates), normal(normal)
		{
		}



	}SpotLightingMaterialVertex;


	class Effect;
	class SpotLightMaterial : public Material
	{
		RTTI_DECLARATIONS(SpotLightMaterial, Material);

		MATERIAL_VARIABLE_DECLARATION(AmbientColor);
		MATERIAL_VARIABLE_DECLARATION(LightColor);
		MATERIAL_VARIABLE_DECLARATION(LightPosition);
		MATERIAL_VARIABLE_DECLARATION(LightLookAt);
		MATERIAL_VARIABLE_DECLARATION(LightRadius);
		MATERIAL_VARIABLE_DECLARATION(SpotLightInnerAngle);
		MATERIAL_VARIABLE_DECLARATION(SpotLightOuterAngle);
		MATERIAL_VARIABLE_DECLARATION(CameraPosition);
		MATERIAL_VARIABLE_DECLARATION(WorldViewProjection);
		MATERIAL_VARIABLE_DECLARATION(World);
		MATERIAL_VARIABLE_DECLARATION(SpecularColor);
		MATERIAL_VARIABLE_DECLARATION(SpecularPower);
		MATERIAL_VARIABLE_DECLARATION(ColorTexture);





	public:
		SpotLightMaterial();
		~SpotLightMaterial();
		void initialize(Effect * effect) override;

		//virtual void createVertexBuffer(ID3D11Device * device, const Model& model, std::vector<ID3D11Buffer*> & vertexBuffers)const;
		virtual void createVertexBuffer(ID3D11Device * device, const Mesh& model, ID3D11Buffer** vertexBuffers)const override;
		void createVertexBuffer(ID3D11Device * device, SpotLightingMaterialVertex * vertices, UINT vertexCount, ID3D11Buffer ** vertexBuffer)const;
		virtual UINT vertexSize()const;


	private:
	};

}
