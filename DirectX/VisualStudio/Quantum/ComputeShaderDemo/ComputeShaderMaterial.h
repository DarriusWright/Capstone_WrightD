#pragma once
#include <Materials\Material.h>

using namespace Library;


typedef struct _VertexPositionTexture
{
	XMFLOAT4 Position;
	XMFLOAT2 TextureCoordinates;

	_VertexPositionTexture() { }

	_VertexPositionTexture(const XMFLOAT4& position, const XMFLOAT2& textureCoordinates)
		: Position(position), TextureCoordinates(textureCoordinates) { }
} VertexPositionTexture;

class ComputeShaderMaterial : public Material
{
	RTTI_DECLARATIONS(ComputeShaderMaterial, Material);

	MATERIAL_VARIABLE_DECLARATION(TextureSize);
	MATERIAL_VARIABLE_DECLARATION(BlueColor);
	MATERIAL_VARIABLE_DECLARATION(ColorTexture);
	MATERIAL_VARIABLE_DECLARATION(OutputTexture);

public:
	void initialize(Effect * effect);
	virtual void createVertexBuffer(ID3D11Device * device, const Mesh& model, ID3D11Buffer** vertexBuffers)const;
	virtual UINT vertexSize()const;
	void createVertexBuffer(ID3D11Device* device, VertexPositionTexture* vertices, UINT vertexCount, ID3D11Buffer** vertexBuffer) const;


public:
	ComputeShaderMaterial();
	~ComputeShaderMaterial();
};

