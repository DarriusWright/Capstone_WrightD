#pragma once

#include <Components\DrawableGameComponent.h>
#include <Camera\Camera.h>
#include <d3dcompiler.h>
#include <Misc\Colors.h>
namespace Library
{

	class TriangleDemo : public DrawableGameComponent
	{
		RTTI_DECLARATIONS(TriangleDemo, DrawableGameComponent);
	public:
		TriangleDemo(Game & game, Camera & camera);
		~TriangleDemo();

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

		TriangleDemo();
		TriangleDemo(const TriangleDemo & rhs);
		TriangleDemo& operator=(const TriangleDemo & rhs);

		ID3DX11Effect * mEffect;
		ID3DX11EffectTechnique * mTechnique;
		ID3DX11EffectPass * mPass;
		ID3DX11EffectMatrixVariable * mWvpVariable;
		ID3D11InputLayout * mInputLayout;
		ID3D11Buffer * mVertexBuffer;
		XMFLOAT4X4 mWorldMatrix;
		FLOAT mAngle;
	};
}
