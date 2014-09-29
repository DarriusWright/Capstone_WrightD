#pragma once

#include <Materials\Material.h>

namespace Library
{

	typedef struct _BasicMaterialVertex
	{
		XMFLOAT4 position;
		XMFLOAT4 color;

		_BasicMaterialVertex(){}
		_BasicMaterialVertex(const XMFLOAT4& position, const XMFLOAT4 & color) : position(position), color(color)
		{

		}
	}BasicMaterialVertex;

	class BasicMaterial : public Material
	{

		RTTI_DECLARATIONS(BasicMaterial, Material);
		MATERIAL_VARIABLE_DECLARATION(WorldViewProjection);
	public:
		BasicMaterial();
		~BasicMaterial();
		UINT vertexSize()const override;
		virtual void initialize(Effect * effect)override;
		
		 void createVertexBuffer(ID3D11Device * device, BasicMaterialVertex * vertices,UINT vertexCount, ID3D11Buffer** vertexBuffers)const;

		virtual void createVertexBuffer(ID3D11Device * device, const Mesh & mesh, ID3D11Buffer ** vertexBuffer)const override;
	};

}