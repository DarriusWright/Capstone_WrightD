#include "DiffuseLightingMaterial.h"
#include <3DObjects\Mesh.h>
#include <Exceptions\GameException.h>

namespace Library
{

	RTTI_DEFINITIONS(DiffuseLightingMaterial)

	DiffuseLightingMaterial::DiffuseLightingMaterial() :Material("main11"),
		MATERIAL_VARIABLE_INITIALIZATION(WorldViewProjection),
		MATERIAL_VARIABLE_INITIALIZATION(World),
		MATERIAL_VARIABLE_INITIALIZATION(AmbientColor),
		MATERIAL_VARIABLE_INITIALIZATION(LightColor),
		MATERIAL_VARIABLE_INITIALIZATION(LightDirection),
		MATERIAL_VARIABLE_INITIALIZATION(ColorTexture)


	{
	}


	MATERIAL_VARIABLE_DEFINITION(DiffuseLightingMaterial, WorldViewProjection);
	MATERIAL_VARIABLE_DEFINITION(DiffuseLightingMaterial, World);
	MATERIAL_VARIABLE_DEFINITION(DiffuseLightingMaterial, AmbientColor);
	MATERIAL_VARIABLE_DEFINITION(DiffuseLightingMaterial, LightColor);
	MATERIAL_VARIABLE_DEFINITION(DiffuseLightingMaterial, LightDirection);
	MATERIAL_VARIABLE_DEFINITION(DiffuseLightingMaterial, ColorTexture);


	DiffuseLightingMaterial::~DiffuseLightingMaterial()
	{

	}



	void DiffuseLightingMaterial::initialize(Effect * effect)
	{
		Material::initialize(effect);

		MATERIAL_VARIABLE_RETRIEVE(WorldViewProjection);
		MATERIAL_VARIABLE_RETRIEVE(World);
		MATERIAL_VARIABLE_RETRIEVE(AmbientColor);
		MATERIAL_VARIABLE_RETRIEVE(LightColor);
		MATERIAL_VARIABLE_RETRIEVE(LightDirection);
		MATERIAL_VARIABLE_RETRIEVE(ColorTexture);

		D3D11_INPUT_ELEMENT_DESC inputElementDescrptions[] = 
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		createInputLayout("main11", "p0", inputElementDescrptions, ARRAYSIZE(inputElementDescrptions));

	}
	void DiffuseLightingMaterial::createVertexBuffer(ID3D11Device * device, const Mesh & mesh, ID3D11Buffer ** vertexBuffer)const
	{
		const std::vector<XMFLOAT3> & sourceVerts = mesh.vertices();
		const std::vector<XMFLOAT3> * sourceUvs = mesh.textureCoords().at(0);
		assert(sourceVerts.size() == mesh.textureCoords().at(0)->size());
		const std::vector<XMFLOAT3> & sourceNormals = mesh.normals();
		assert(sourceVerts.size() == mesh.normals().size());


		std::vector<DiffuseLightingMaterialVertex> vertices;

		vertices.reserve(sourceVerts.size());

		for (UINT i = 0; i < sourceVerts.size(); i++)
		{
			XMFLOAT3 position = sourceVerts.at(i);
			XMFLOAT3 uv = sourceUvs->at(i);
			XMFLOAT3 normal = sourceUvs->at(i);

			vertices.push_back(DiffuseLightingMaterialVertex(XMFLOAT4(position.x, position.y, position.z, 1.0f), XMFLOAT2(uv.x, uv.y), normal));
		}

		createVertexBuffer(device, &vertices[0], vertices.size(), vertexBuffer);


	}
	void DiffuseLightingMaterial::createVertexBuffer(ID3D11Device * device, DiffuseLightingMaterialVertex * vertices, UINT vertexCount, ID3D11Buffer ** vertexBuffer)const
	{

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.ByteWidth = vertexCount * vertexSize();
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

		D3D11_SUBRESOURCE_DATA vertexSubResource;
		ZeroMemory(&vertexSubResource, sizeof(vertexSubResource));
		vertexSubResource.pSysMem = vertices;
		if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexSubResource, vertexBuffer)))
		{
			throw GameException(L"CreateBuffer Failed ");
		}

	}
	UINT DiffuseLightingMaterial::vertexSize() const
	{
		return sizeof(DiffuseLightingMaterialVertex);
	}

}