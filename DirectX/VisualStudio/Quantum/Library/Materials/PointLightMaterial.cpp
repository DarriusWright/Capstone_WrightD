#include "PointLightMaterial.h"
#include <3DObjects\Mesh.h>
#include <Exceptions\GameException.h>

namespace Library
{

	

	RTTI_DEFINITIONS(PointLightMaterial, Material);

	PointLightMaterial::PointLightMaterial() :Material("main11"),
	MATERIAL_VARIABLE_INITIALIZATION(WorldViewProjection),
	MATERIAL_VARIABLE_INITIALIZATION(World),
	MATERIAL_VARIABLE_INITIALIZATION(AmbientColor),
	MATERIAL_VARIABLE_INITIALIZATION(SpecularColor),
	MATERIAL_VARIABLE_INITIALIZATION(SpecularPower),
	MATERIAL_VARIABLE_INITIALIZATION(LightColor),
	MATERIAL_VARIABLE_INITIALIZATION(LightPosition),
	MATERIAL_VARIABLE_INITIALIZATION(CameraPosition),
	MATERIAL_VARIABLE_INITIALIZATION(LightRadius),
	MATERIAL_VARIABLE_INITIALIZATION(ColorTexture)


	{
	}


	//type checking
	//put any variables you need 
	MATERIAL_VARIABLE_DEFINITION(PointLightMaterial, WorldViewProjection);
	MATERIAL_VARIABLE_DEFINITION(PointLightMaterial, World);
	MATERIAL_VARIABLE_DEFINITION(PointLightMaterial, AmbientColor);
	MATERIAL_VARIABLE_DEFINITION(PointLightMaterial, SpecularColor);
	MATERIAL_VARIABLE_DEFINITION(PointLightMaterial, SpecularPower);
	MATERIAL_VARIABLE_DEFINITION(PointLightMaterial, LightColor);
	MATERIAL_VARIABLE_DEFINITION(PointLightMaterial, LightPosition);
	MATERIAL_VARIABLE_DEFINITION(PointLightMaterial, CameraPosition);
	MATERIAL_VARIABLE_DEFINITION(PointLightMaterial, LightRadius);
	MATERIAL_VARIABLE_DEFINITION(PointLightMaterial, ColorTexture);


	PointLightMaterial::~PointLightMaterial()
	{
	}


	void PointLightMaterial::initialize(Effect * effect)
	{
		Material::initialize(effect);

		MATERIAL_VARIABLE_RETRIEVE(WorldViewProjection);
		MATERIAL_VARIABLE_RETRIEVE(World);
		MATERIAL_VARIABLE_RETRIEVE(AmbientColor);
		MATERIAL_VARIABLE_RETRIEVE(SpecularColor);
		MATERIAL_VARIABLE_RETRIEVE(SpecularPower);
		MATERIAL_VARIABLE_RETRIEVE(LightColor);
		MATERIAL_VARIABLE_RETRIEVE(LightPosition);
		MATERIAL_VARIABLE_RETRIEVE(CameraPosition);
		MATERIAL_VARIABLE_RETRIEVE(LightRadius);
		MATERIAL_VARIABLE_RETRIEVE(ColorTexture);

		D3D11_INPUT_ELEMENT_DESC inputElementDescrptions[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		createInputLayout("main11", "p0", inputElementDescrptions, ARRAYSIZE(inputElementDescrptions));

	}
	void PointLightMaterial::createVertexBuffer(ID3D11Device* device, const Mesh& mesh, ID3D11Buffer** vertexBuffer) const
	{


		const std::vector<XMFLOAT3>& sourceVertices = mesh.vertices();
			const std::vector<XMFLOAT3> * sourceUvs = mesh.textureCoords().at(0);
		std::vector<XMFLOAT3>* textureCoordinates = mesh.textureCoords().at(0);
		assert(textureCoordinates->size() == sourceVertices.size());

		const std::vector<XMFLOAT3>& normals = mesh.normals();
		assert(normals.size() == sourceVertices.size());

		std::vector<PointLightMaterialVertex> vertices;


			for (UINT i = 0; i < sourceVertices.size(); i++)
			{
				XMFLOAT3 position = sourceVertices.at(i);
				XMFLOAT3 uv = sourceUvs->at(i);
				XMFLOAT3 normal = normals.at(i);

				vertices.push_back(PointLightMaterialVertex(XMFLOAT4(position.x, position.y, position.z, 1.0f), XMFLOAT2(uv.x, uv.y), normal));
			}

			createVertexBuffer(device, &vertices[0], vertices.size(), vertexBuffer);


	}

	void PointLightMaterial::createVertexBuffer(ID3D11Device * device, PointLightMaterialVertex * vertices, UINT vertexCount, ID3D11Buffer ** vertexBuffer)const
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
	UINT PointLightMaterial::vertexSize() const
	{
		return sizeof(PointLightMaterialVertex);
	}

}