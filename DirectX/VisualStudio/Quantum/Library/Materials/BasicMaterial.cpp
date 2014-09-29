#include "BasicMaterial.h"

#include <Exceptions\GameException.h>
#include <3DObjects\Mesh.h>
#include <Misc\Colors.h>

namespace Library
{
	RTTI_DEFINITIONS(BasicMaterial);

	BasicMaterial::BasicMaterial() :Material("main11"), MATERIAL_VARIABLE_INITIALIZATION(WorldViewProjection)
	{
	}


	BasicMaterial::~BasicMaterial()
	{
	}
	MATERIAL_VARIABLE_DEFINITION(BasicMaterial, WorldViewProjection);


	void BasicMaterial::initialize(Effect * effect)
	{
		Material::initialize(effect);
		MATERIAL_VARIABLE_RETRIEVE(WorldViewProjection);
		D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		createInputLayout("main11", "p0", inputElementDescriptions, ARRAYSIZE(inputElementDescriptions));
	}
	void BasicMaterial::createVertexBuffer(ID3D11Device * device, const Mesh & mesh, ID3D11Buffer ** vertexBuffer)const
	{
		const std::vector<XMFLOAT3>& sourceVerts = mesh.vertices();
		std::vector<BasicMaterialVertex> vertices;
		vertices.reserve(sourceVerts.size());

		if (mesh.vertexColors().size() > 0)
		{
			std::vector<XMFLOAT4> * vertexColors = mesh.vertexColors().at(0);
			assert(vertexColors->size() == sourceVerts.size());
			for (UINT i = 0; i < sourceVerts.size(); i++)
			{
				XMFLOAT3 position = sourceVerts.at(i);
				XMFLOAT4 color = vertexColors->at(i);

				vertices.push_back(BasicMaterialVertex(XMFLOAT4(position.x, position.y, position.z, 1.0f), color));
			}
		}
		else
		{

			XMFLOAT4 color = XMFLOAT4(reinterpret_cast<float*>(&Colors::WHITE));

			for (UINT i = 0; i < sourceVerts.size(); i++)
			{
				XMFLOAT3 position = sourceVerts.at(i);

				vertices.push_back(BasicMaterialVertex(XMFLOAT4(position.x, position.y, position.z, 1.0f), color));
			}
		}

		createVertexBuffer(device, &vertices[0],vertices.size(), vertexBuffer);

	}

	void BasicMaterial::createVertexBuffer(ID3D11Device * device, BasicMaterialVertex * vertices, UINT vertexCount, ID3D11Buffer** vertexBuffers)const
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


	UINT BasicMaterial::vertexSize()const
	{
		return sizeof(BasicMaterialVertex);
	}

}
