#include "ComputeShaderMaterial.h"
#include <3DObjects\Mesh.h>
#include <Exceptions\GameException.h>


RTTI_DEFINITIONS(ComputeShaderMaterial)
ComputeShaderMaterial::ComputeShaderMaterial() : Material("compute"),
MATERIAL_VARIABLE_INITIALIZATION(TextureSize),
MATERIAL_VARIABLE_INITIALIZATION(BlueColor),
MATERIAL_VARIABLE_INITIALIZATION(ColorTexture),
MATERIAL_VARIABLE_INITIALIZATION(OutputTexture)

{
}

MATERIAL_VARIABLE_DEFINITION(ComputeShaderMaterial,TextureSize);
MATERIAL_VARIABLE_DEFINITION(ComputeShaderMaterial, BlueColor);
MATERIAL_VARIABLE_DEFINITION(ComputeShaderMaterial, ColorTexture);
MATERIAL_VARIABLE_DEFINITION(ComputeShaderMaterial, OutputTexture);


ComputeShaderMaterial::~ComputeShaderMaterial()
{
}


void ComputeShaderMaterial::initialize(Effect * effect)
{
	Material::initialize(effect);
	MATERIAL_VARIABLE_RETRIEVE(TextureSize);
	MATERIAL_VARIABLE_RETRIEVE(BlueColor);
	MATERIAL_VARIABLE_RETRIEVE(ColorTexture);
	MATERIAL_VARIABLE_RETRIEVE(OutputTexture);

	D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	createInputLayout("render", "p0", inputElementDescriptions, ARRAYSIZE(inputElementDescriptions));

}

void ComputeShaderMaterial::createVertexBuffer(ID3D11Device * device, const Mesh& mesh, ID3D11Buffer** vertexBuffers)const
{
	const std::vector<XMFLOAT3>& sourceVertices = mesh.vertices();
	std::vector<XMFLOAT3>* textureCoordinates = mesh.textureCoords().at(0);
	assert(textureCoordinates->size() == sourceVertices.size());

	std::vector<VertexPositionTexture> vertices;
	vertices.reserve(sourceVertices.size());
	for (UINT i = 0; i < sourceVertices.size(); i++)
	{
		XMFLOAT3 position = sourceVertices.at(i);
		XMFLOAT3 uv = textureCoordinates->at(i);
		vertices.push_back(VertexPositionTexture(XMFLOAT4(position.x, position.y, position.z, 1.0f), XMFLOAT2(uv.x, uv.y)));
	}

	createVertexBuffer(device, &vertices[0], vertices.size(), vertexBuffers);
}


void ComputeShaderMaterial::createVertexBuffer(ID3D11Device* device, VertexPositionTexture* vertices, UINT vertexCount, ID3D11Buffer** vertexBuffer) const
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.ByteWidth = vertexSize() * vertexCount;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexSubResourceData;
	ZeroMemory(&vertexSubResourceData, sizeof(vertexSubResourceData));
	vertexSubResourceData.pSysMem = vertices;
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, vertexBuffer)))
	{
		throw GameException("ID3D11Device::CreateBuffer() failed.");
	}
}

UINT ComputeShaderMaterial::vertexSize()const{ 
	return sizeof(VertexPositionTexture); }