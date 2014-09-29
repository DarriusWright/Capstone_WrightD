#pragma once
#include <Misc\Common.h>
#include <Materials\Material.h>
namespace Library
{

	typedef struct _PointLightMaterialVertex
	{
		XMFLOAT4 position;
		XMFLOAT2 textureCoordinates;
		XMFLOAT3 normal;

		_PointLightMaterialVertex(){}
		_PointLightMaterialVertex(XMFLOAT4 position,
			XMFLOAT2 textureCoordinates,
			XMFLOAT3 normal) : position(position), textureCoordinates(textureCoordinates), normal(normal)
		{
		}



	}PointLightMaterialVertex;

	class PointLightMaterial : public Material
	{


		//type checking
		RTTI_DECLARATIONS(PointLightMaterial, Material);
		//put any variables you need 
		MATERIAL_VARIABLE_DECLARATION(WorldViewProjection);
		MATERIAL_VARIABLE_DECLARATION(World);
		MATERIAL_VARIABLE_DECLARATION(AmbientColor);
		MATERIAL_VARIABLE_DECLARATION(SpecularColor);
		MATERIAL_VARIABLE_DECLARATION(SpecularPower);
		MATERIAL_VARIABLE_DECLARATION(LightColor);
		MATERIAL_VARIABLE_DECLARATION(LightPosition);
		MATERIAL_VARIABLE_DECLARATION(CameraPosition);
		MATERIAL_VARIABLE_DECLARATION(LightRadius);
		MATERIAL_VARIABLE_DECLARATION(ColorTexture);

	public:
		PointLightMaterial();
		~PointLightMaterial();

		virtual void initialize(Effect * effect)override;
		virtual void createVertexBuffer(ID3D11Device * device, const Mesh & mesh, ID3D11Buffer ** vertexBuffer)const override;
		void createVertexBuffer(ID3D11Device * device, PointLightMaterialVertex * vertices, UINT vertexCount, ID3D11Buffer ** vertexBuffer)const;
		virtual UINT vertexSize() const override;
	};

}