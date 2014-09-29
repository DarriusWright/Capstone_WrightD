#include "SkyboxMaterial.h"
#include <3DObjects\Mesh.h>
#include <Exceptions\GameException.h>
namespace Library
{

	RTTI_DEFINITIONS(SkyboxMaterial);


	SkyboxMaterial::SkyboxMaterial() : Material("main11"),
		MATERIAL_VARIABLE_INITIALIZATION(WorldViewProjection), MATERIAL_VARIABLE_INITIALIZATION(SkyboxTexture)
	{
	}

	MATERIAL_VARIABLE_DEFINITION(SkyboxMaterial, WorldViewProjection)
		MATERIAL_VARIABLE_DEFINITION(SkyboxMaterial, SkyboxTexture)
		SkyboxMaterial::~SkyboxMaterial()
	{
		}

	void SkyboxMaterial::initialize(Effect * effect)
	{
		Material::initialize(effect);

		MATERIAL_VARIABLE_RETRIEVE(WorldViewProjection);
		MATERIAL_VARIABLE_RETRIEVE(SkyboxTexture);


		D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		createInputLayout("main11", "p0", inputElementDescriptions, ARRAYSIZE(inputElementDescriptions));


	}

	void SkyboxMaterial::createVertexBuffer(ID3D11Device * device, const Mesh & mesh, ID3D11Buffer ** vertexBuffer)const
	{
		const std::vector<XMFLOAT3>& sourceVerts = mesh.vertices();
		std::vector<XMFLOAT4> vertices;
		vertices.reserve(sourceVerts.size());


		for (UINT i = 0; i < sourceVerts.size(); i++)
		{
			XMFLOAT3 position = sourceVerts.at(i);

			vertices.push_back(XMFLOAT4(position.x, position.y, position.z, 1.0f));
		}


		createVertexBuffer(device, &vertices[0], vertices.size(), vertexBuffer);

	}

	void SkyboxMaterial::createVertexBuffer(ID3D11Device * device, XMFLOAT4 * vertices, UINT vertexCount, ID3D11Buffer** vertexBuffers)const
	{
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.ByteWidth = vertexCount * vertexSize();
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

		D3D11_SUBRESOURCE_DATA vertexSubResource;
		ZeroMemory(&vertexSubResource, sizeof(vertexSubResource));
		vertexSubResource.pSysMem = vertices;
		if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexSubResource, vertexBuffers)))
		{
			throw GameException(L"CreateBuffer Failed ");
		}
	}


	UINT SkyboxMaterial::vertexSize()const
	{
		return sizeof(XMFLOAT4);
	}
}