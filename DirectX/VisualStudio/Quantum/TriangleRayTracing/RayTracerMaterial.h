#pragma once

#include <Materials\Material.h>
#include <Vertex\VertexPositionTexture.h>

using namespace Library;


class RayTracerMaterial :public Material

{
	RTTI_DECLARATIONS(RayTracerMaterial, Material);

	MATERIAL_VARIABLE_DECLARATION(TextureSize);
	MATERIAL_VARIABLE_DECLARATION(Color);
	MATERIAL_VARIABLE_DECLARATION(ColorTexture);
	MATERIAL_VARIABLE_DECLARATION(OutputTexture);
	MATERIAL_VARIABLE_DECLARATION(DepthTexture);
	MATERIAL_VARIABLE_DECLARATION(CameraPosition);
	MATERIAL_VARIABLE_DECLARATION(CameraDirection);
	MATERIAL_VARIABLE_DECLARATION(Position);
	MATERIAL_VARIABLE_DECLARATION(LightPosition);
	MATERIAL_VARIABLE_DECLARATION(Eye);
	MATERIAL_VARIABLE_DECLARATION(Distance);
	MATERIAL_VARIABLE_DECLARATION(Normal);
	MATERIAL_VARIABLE_DECLARATION(U);
	MATERIAL_VARIABLE_DECLARATION(V);
	MATERIAL_VARIABLE_DECLARATION(W);
	MATERIAL_VARIABLE_DECLARATION(v0);
	MATERIAL_VARIABLE_DECLARATION(v1);
	MATERIAL_VARIABLE_DECLARATION(v2);

public:
	void initialize(Effect * effect);
	virtual void createVertexBuffer(ID3D11Device * device, const Mesh& model, ID3D11Buffer** vertexBuffers)const;
	virtual UINT vertexSize()const;
	void createVertexBuffer(ID3D11Device* device, VertexPositionTexture * vertices, UINT vertexCount, ID3D11Buffer** vertexBuffer) const;


public:
	RayTracerMaterial();
	~RayTracerMaterial();
};

