#pragma once

#include <Materials\Material.h>
#include <Vertex\VertexPositionTexture.h>

using namespace Library;


class RayTracerMaterial :public Material

{
	RTTI_DECLARATIONS(RayTracerMaterial, Material);

	MATERIAL_VARIABLE_DECLARATION(TextureSize);
	MATERIAL_VARIABLE_DECLARATION(BlueColor);
	MATERIAL_VARIABLE_DECLARATION(ColorTexture);
	MATERIAL_VARIABLE_DECLARATION(OutputTexture);
	MATERIAL_VARIABLE_DECLARATION(CameraPosition);
	MATERIAL_VARIABLE_DECLARATION(ProjectionMatrix);
	MATERIAL_VARIABLE_DECLARATION(ViewMatrix);
	MATERIAL_VARIABLE_DECLARATION(InverseProjectionMatrix);
	MATERIAL_VARIABLE_DECLARATION(InverseViewMatrix);
	MATERIAL_VARIABLE_DECLARATION(Position);
	MATERIAL_VARIABLE_DECLARATION(LightPosition);

public:
	void initialize(Effect * effect);
	virtual void createVertexBuffer(ID3D11Device * device, const Mesh& model, ID3D11Buffer** vertexBuffers)const;
	virtual UINT vertexSize()const;
	void createVertexBuffer(ID3D11Device* device, VertexPositionTexture * vertices, UINT vertexCount, ID3D11Buffer** vertexBuffer) const;


public:
	RayTracerMaterial();
	~RayTracerMaterial();
};

