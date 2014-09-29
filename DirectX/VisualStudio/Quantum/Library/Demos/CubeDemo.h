#pragma once

#include <Components\DrawableGameComponent.h>
#include <Camera\Camera.h>
#include <d3dcompiler.h>
#include <Misc\Colors.h>
namespace Library
{

	class CubeDemo : public DrawableGameComponent
	{
		RTTI_DECLARATIONS(CubeDemo, DrawableGameComponent);
	public:
		CubeDemo(Game & game, Camera & camera);
		~CubeDemo();

		void initialize()override;
		void draw(const GameTimer & timer)override;
		void update(const GameTimer & timer)override;

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

		CubeDemo();
		CubeDemo(const CubeDemo & rhs);
		CubeDemo& operator=(const CubeDemo & rhs);

		ID3DX11Effect * mEffect;
		ID3DX11EffectTechnique * mTechnique;
		ID3DX11EffectPass * mPass;
		ID3DX11EffectMatrixVariable * mWvpVariable;
		ID3D11InputLayout * mInputLayout;
		ID3D11Buffer * mVertexBuffer;
		ID3D11Buffer * mIndexBuffer;
		XMFLOAT4X4 mWorldMatrix;
		FLOAT mAngle;
	};
}
