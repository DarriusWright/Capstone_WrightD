#pragma once
#include <Misc\Common.h>
#include <Components\GameComponent.h>
#include <Components\DrawableGameComponent.h>
namespace Library
{
	class VertexManager;
	class IndexManager;
	class Game;
	class Mesh;
	class Camera;
	class MeshManager : public DrawableGameComponent 
	{
		RTTI_DECLARATIONS(MeshManager, DrawableGameComponent);
	public:
		
		MeshManager(Game & game,Camera & camera, ID3D11InputLayout * inputLayout, ID3DX11EffectPass * pass, ID3DX11EffectMatrixVariable * mvpVariable);
		~MeshManager();
		void initialize() override;
		void update(const GameTimer & gameTimer) override;
		void draw(const GameTimer & gameTimer)override;
		void addMesh(Mesh * mesh);
		std::vector<Mesh *> & meshes();
		std::vector<VertexManager *> & vertexManagers();
		void setDrawInformation(ID3D11InputLayout * inputLayout, ID3DX11EffectPass * pass, ID3DX11EffectMatrixVariable * mvpVariable);
		ID3D11ShaderResourceView * mTextureShaderResourceView;
		ID3DX11EffectShaderResourceVariable * mColorTextureVarialble;
	private:
		ID3D11InputLayout * mInputLayout;
		ID3DX11EffectMatrixVariable * mMvpVariable;
		ID3DX11EffectPass * mPass;
		VertexManager * getVectorManager(UINT size);
		IndexManager * getIndexManager(UINT size);
		std::vector <VertexManager * > mVectorManagers;
		std::vector <IndexManager * > mIndexManagers;
		std::vector<Mesh *>mMeshes;
	};

}