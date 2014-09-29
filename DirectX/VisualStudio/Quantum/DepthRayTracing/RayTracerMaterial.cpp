#include "RayTracerMaterial.h"
#include <3DObjects\Mesh.h>
#include <Exceptions\GameException.h>


RTTI_DEFINITIONS(RayTracerMaterial)
RayTracerMaterial::RayTracerMaterial() : Material("compute"),
MATERIAL_VARIABLE_INITIALIZATION(TextureSize),
MATERIAL_VARIABLE_INITIALIZATION(Color),
MATERIAL_VARIABLE_INITIALIZATION(ColorTexture),
MATERIAL_VARIABLE_INITIALIZATION(CameraPosition),
MATERIAL_VARIABLE_INITIALIZATION(OutputTexture),
MATERIAL_VARIABLE_INITIALIZATION(DepthTexture),
MATERIAL_VARIABLE_INITIALIZATION(ViewMatrix),
MATERIAL_VARIABLE_INITIALIZATION(ProjectionMatrix),
MATERIAL_VARIABLE_INITIALIZATION(InverseViewMatrix),
MATERIAL_VARIABLE_INITIALIZATION(InverseProjectionMatrix),
MATERIAL_VARIABLE_INITIALIZATION(Position),
MATERIAL_VARIABLE_INITIALIZATION(LightPosition)

{
}


MATERIAL_VARIABLE_DEFINITION(RayTracerMaterial, TextureSize);
MATERIAL_VARIABLE_DEFINITION(RayTracerMaterial, Color);
MATERIAL_VARIABLE_DEFINITION(RayTracerMaterial, ColorTexture);
MATERIAL_VARIABLE_DEFINITION(RayTracerMaterial, OutputTexture);
MATERIAL_VARIABLE_DEFINITION(RayTracerMaterial, DepthTexture);
MATERIAL_VARIABLE_DEFINITION(RayTracerMaterial, CameraPosition);
MATERIAL_VARIABLE_DEFINITION(RayTracerMaterial, ViewMatrix);
MATERIAL_VARIABLE_DEFINITION(RayTracerMaterial, ProjectionMatrix);
MATERIAL_VARIABLE_DEFINITION(RayTracerMaterial, InverseViewMatrix);
MATERIAL_VARIABLE_DEFINITION(RayTracerMaterial, InverseProjectionMatrix);
MATERIAL_VARIABLE_DEFINITION(RayTracerMaterial, Position);
MATERIAL_VARIABLE_DEFINITION(RayTracerMaterial, LightPosition);


RayTracerMaterial::~RayTracerMaterial()
{
}


void RayTracerMaterial::initialize(Effect * effect)
{
	Material::initialize(effect);
	MATERIAL_VARIABLE_RETRIEVE(TextureSize);
	MATERIAL_VARIABLE_RETRIEVE(Color);
	MATERIAL_VARIABLE_RETRIEVE(ColorTexture);
	MATERIAL_VARIABLE_RETRIEVE(OutputTexture);
	MATERIAL_VARIABLE_RETRIEVE(DepthTexture);
	MATERIAL_VARIABLE_RETRIEVE(CameraPosition);
	MATERIAL_VARIABLE_RETRIEVE(ViewMatrix);
	MATERIAL_VARIABLE_RETRIEVE(ProjectionMatrix);
	MATERIAL_VARIABLE_RETRIEVE(InverseViewMatrix);
	MATERIAL_VARIABLE_RETRIEVE(InverseProjectionMatrix);
	MATERIAL_VARIABLE_RETRIEVE(LightPosition);
	MATERIAL_VARIABLE_RETRIEVE(Position);

	D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	createInputLayout("render", "p0", inputElementDescriptions, ARRAYSIZE(inputElementDescriptions));

}

void RayTracerMaterial::createVertexBuffer(ID3D11Device * device, const Mesh& mesh, ID3D11Buffer** vertexBuffers)const
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


void RayTracerMaterial::createVertexBuffer(ID3D11Device* device, VertexPositionTexture* vertices, UINT vertexCount, ID3D11Buffer** vertexBuffer) const
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

UINT RayTracerMaterial::vertexSize()const{
	return sizeof(VertexPositionTexture);
}