#include "MeshManager.h"

#include <3DObjects\Model.h>
#include <3DObjects\Mesh.h>
#include <DataManagement\VertexManager.h>
#include <DataManagement\IndexManager.h>


namespace Library
{
	RTTI_DEFINITIONS(MeshManager);

	MeshManager::MeshManager(Game & game, Camera & camera, ID3D11InputLayout * inputLayout, ID3DX11EffectPass * pass, ID3DX11EffectMatrixVariable * mvpVariable) : DrawableGameComponent(game, camera)
		, mPass(pass), mMvpVariable(mvpVariable), mInputLayout(inputLayout)
	{

	}


	MeshManager::~MeshManager()
	{
		for (VertexManager * mg : mVectorManagers)
		{
			DeleteObject(mg);
		}

		for (IndexManager * mg : mIndexManagers)
		{
			DeleteObject(mg);
		}

	}

	std::vector<Mesh *> & MeshManager::meshes()
	{
		return mMeshes;
	}

	void MeshManager::draw(const GameTimer & gameTimer)
	{
		ID3D11DeviceContext * deviceContext = mGame->deviceContext();
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->IASetInputLayout(mInputLayout);

		UINT stride = sizeof(TextureMappingVertex);
		UINT offset = 0;


		for (Mesh * mesh : mMeshes)
		{

			ID3D11Buffer * vertexBuffer = mesh->getVertexBuffer();
			ID3D11Buffer * indexBuffer = mesh->getIndexBuffer();

			mColorTextureVarialble->SetResource(mesh->getModel().textureShaderResourceView());
			
			
			UINT vertexOffset = mesh->getVertexOffset();
			UINT indexOffset = mesh->getIndexOffset() ;
			deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			//deviceContext->IASetInputLayout(mInputLayout);

			XMMATRIX worldMatrix = XMLoadFloat4x4(&mesh->getWorldMatrix());
			XMMATRIX wvp =worldMatrix * mCamera->viewMatrix() * mCamera->projectionMatrix();
			mMvpVariable->SetMatrix(reinterpret_cast<float*>(&wvp));

			mPass->Apply(0, deviceContext);
			deviceContext->DrawIndexed(mesh->indices().size(),mesh->getIndexOffset()/sizeof(UINT),mesh->getVertexOffset()/sizeof(TextureMappingVertex));
		}



	}


	void MeshManager::setDrawInformation(ID3D11InputLayout * inputLayout, ID3DX11EffectPass * pass, ID3DX11EffectMatrixVariable * mvpVariable)
	{
		mInputLayout = inputLayout;
		mPass = pass;
		mMvpVariable = mvpVariable;
	}


	void MeshManager::initialize()
	{
		DrawableGameComponent::initialize();
	}
	void MeshManager::update(const GameTimer & gameTimer)
	{

	}
	void MeshManager::addMesh(Mesh * mesh)
	{
		VertexManager * manager = getVectorManager(mesh->vertices().size());
		manager->addMesh(mesh);
		IndexManager * indexManager = getIndexManager(mesh->indices().size());
		indexManager->addMesh(mesh);
		mMeshes.push_back(mesh);
	}

	std::vector<VertexManager *> & MeshManager::vertexManagers()
	{
		return mVectorManagers;
	}


	VertexManager * MeshManager::getVectorManager(UINT size)
	{
		VertexManager * manager = nullptr;
		for (size_t i = 0; i < mVectorManagers.size() && manager == nullptr; i++)
		{
			if (mVectorManagers[i]->spaceLeft() >  size)
			{
				manager = mVectorManagers[i];
			}
		}

		if (manager == nullptr)
		{
			manager = new VertexManager(*mGame);
			manager->initialize();
			mVectorManagers.push_back(manager);
		}

		return manager;
	}
	IndexManager * MeshManager::getIndexManager(UINT size)
	{
		IndexManager * manager = nullptr;
		for (size_t i = 0; i < mIndexManagers.size() && manager == nullptr; i++)
		{
			if (mIndexManagers[i]->spaceLeft() >  size)
			{
				manager = mIndexManagers[i];
			}
		}

		if (manager == nullptr)
		{
			manager = new IndexManager(*mGame);
			manager->initialize();
			mIndexManagers.push_back(manager);
		}

		return manager;

	}
}