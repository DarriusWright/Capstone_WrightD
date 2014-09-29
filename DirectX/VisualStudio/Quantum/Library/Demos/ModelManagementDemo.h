#pragma once

#include <Components\DrawableGameComponent.h>
#include <Camera\Camera.h>
#include <d3dcompiler.h>
#include <Misc\Colors.h>
namespace Library
{
	class Mesh;
	class Model;
	class ModelManagementDemo : public DrawableGameComponent
	{
		RTTI_DECLARATIONS(ModelManagementDemo, DrawableGameComponent);
	public:
		ModelManagementDemo(Game & game, Camera & camera);
		~ModelManagementDemo();

		void initialize()override;
		void draw(const GameTimer & timer)override;
		void update(const GameTimer & timer)override;
		void createVertexBuffer(ID3D11Device * device, Mesh & mesh, ID3D11Buffer ** vertexBuffer)const;

		ID3DX11EffectPass * getPass();
		ID3DX11EffectMatrixVariable * getWvpVariable();
		ID3D11InputLayout * getInputLayout();

	private:
		typedef struct _BasicEffectVertex
		{
			XMFLOAT4 mPosition;
			XMFLOAT4 mColor;

			_BasicEffectVertex()
			{

			}
			_BasicEffectVertex(XMFLOAT4 position, XMFLOAT4 color) : mPosition(position), mColor(color)
			{

			}

		}BasicEffectVertex;

		ModelManagementDemo();
		ModelManagementDemo(const ModelManagementDemo & rhs);
		ModelManagementDemo& operator=(const ModelManagementDemo & rhs);

		UINT mIndexCount;
		ID3DX11Effect * mEffect;
		ID3DX11EffectTechnique * mTechnique;
		ID3DX11EffectPass * mPass;
		ID3DX11EffectMatrixVariable * mWvpVariable;
		ID3D11InputLayout * mInputLayout;
		ID3D11Buffer * mVertexBuffer;
		ID3D11Buffer * mIndexBuffer;
		XMFLOAT4X4 mWorldMatrix;
		ID3D11ShaderResourceView * mTextureShaderResourceView;
		ID3DX11EffectShaderResourceVariable * mColorTextureVarialble;

		FLOAT mAngle;
		Model * model;
		Model * model2;
		Model * model3;
	};
}
