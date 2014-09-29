#pragma once

#include <Materials\Material.h>
#include <Effects\Effect.h>
namespace Library
{

	typedef struct _DiffuseLightingMaterialVertex
	{
		XMFLOAT4 position;
		XMFLOAT2 textureCoordinates;
		XMFLOAT3 normal;

		_DiffuseLightingMaterialVertex(){}
		_DiffuseLightingMaterialVertex(XMFLOAT4 position,
		XMFLOAT2 textureCoordinates,
		XMFLOAT3 normal) : position(position), textureCoordinates(textureCoordinates), normal(normal)
		{
		}



	}DiffuseLightingMaterialVertex;

	class DiffuseLightingMaterial : public Material
	{
		//type checking
		RTTI_DECLARATIONS(DiffuseLightingMaterial, Material);
		//put any variables you need 
		MATERIAL_VARIABLE_DECLARATION(WorldViewProjection);
		MATERIAL_VARIABLE_DECLARATION(World);
		MATERIAL_VARIABLE_DECLARATION(AmbientColor);
		MATERIAL_VARIABLE_DECLARATION(LightColor);
		MATERIAL_VARIABLE_DECLARATION(LightDirection);
		MATERIAL_VARIABLE_DECLARATION(ColorTexture);

	public:
		DiffuseLightingMaterial();
		~DiffuseLightingMaterial();

		virtual void initialize(Effect * effect)override;
		virtual void createVertexBuffer(ID3D11Device * device , const Mesh & mesh, ID3D11Buffer ** vertexBuffer)const override;
		void createVertexBuffer(ID3D11Device * device, DiffuseLightingMaterialVertex * vertices, UINT vertexCount, ID3D11Buffer ** vertexBuffer)const;
		virtual UINT vertexSize() const override;

	};

}