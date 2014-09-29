#pragma once
#include <Misc/Common.h>
#include <Materials\Material.h>

namespace Library
{

	
	class SkyboxMaterial : public Material
	{
		RTTI_DECLARATIONS(SkyboxMaterial, Material);

		MATERIAL_VARIABLE_DECLARATION(WorldViewProjection);
		MATERIAL_VARIABLE_DECLARATION(SkyboxTexture);

	public:
		SkyboxMaterial();
		~SkyboxMaterial();
		virtual void initialize(Effect * effect)override;
		virtual void createVertexBuffer(ID3D11Device * device, const Mesh & mesh, ID3D11Buffer ** vertexBuffer) const override;
		void createVertexBuffer(ID3D11Device* device, XMFLOAT4* vertices, UINT vertexCount, ID3D11Buffer** vertexBuffer) const;
		virtual UINT vertexSize()const override;
	};

}