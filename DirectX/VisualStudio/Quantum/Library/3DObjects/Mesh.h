#pragma once
#include <Misc\Common.h>

struct aiMesh;

namespace Library
{
	class Material;
	class ModelMaterial;
	class VertexManager;
	class Mesh
	{
		friend class Model;
	public:
		Mesh(Model & model, ModelMaterial * material);
		Mesh(Model & model, aiMesh& mesh);

		~Mesh();

		Model& getModel();
		ModelMaterial * getMaterial();
		const std::string & name()const;
		const std::vector<XMFLOAT3>& vertices()const;
		const std::vector<XMFLOAT3>& normals()const;
		const std::vector<XMFLOAT3>& tangents()const;
		const std::vector<XMFLOAT3>& biTangents()const;
		const std::vector<std::vector<XMFLOAT3>*>& textureCoords()const;
		const std::vector<std::vector<XMFLOAT4>*>& vertexColors()const;
		void left();
		void right();
		UINT & getVertexOffset();
		void setVertexOffset(UINT offset);

		UINT & getIndexOffset();
		void setIndexOffset(UINT offset);

		UINT faceCount()const;
		const std::vector<UINT>& indices()const;
		void createIndexBuffer(ID3D11Buffer ** indexBuffer);


		XMFLOAT4X4 & getWorldMatrix();

		ID3D11Buffer * getVertexBuffer();
		ID3D11Buffer * getIndexBuffer();

		void setVertexBuffer(ID3D11Buffer * buffer);
		void setIndexBuffer(ID3D11Buffer * buffer);

	private:
		Mesh(const Mesh& mesh);
		Mesh& operator=(const Mesh& mesh);

		Model & mModel;
		ModelMaterial * mMaterial;
		std::string mName;
		std::vector<XMFLOAT3> mVertices;
		std::vector<XMFLOAT3> mNormals;
		std::vector<XMFLOAT3> mTangents;
		std::vector<XMFLOAT3> mBiTangents;
		std::vector<std::vector<XMFLOAT3>*> mTextureCoords;
		std::vector<std::vector<XMFLOAT4>*> mVertexColors;
		UINT mFaceCount;
		std::vector<UINT> mIndices;

		UINT mVertexOffset;
		UINT mIndexOffset;
		ID3D11Buffer * mVertexBuffer;
		ID3D11Buffer * mIndexBuffer;


		XMFLOAT4X4 mWorldMatrix;
	};

}