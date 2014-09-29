#pragma once
#include <Misc\Common.h>
#include <Effects\Effect.h>
#include <Materials\BasicMaterial.h>
#include <Components\DrawableGameComponent.h>

namespace Library
{

	class ProxyModel : public DrawableGameComponent
	{
		RTTI_DECLARATIONS(ProxyModel, DrawableGameComponent);

	public:


		ProxyModel(Game & game, Camera & camera , const std::string & modelFileName, float scale);
		~ProxyModel();


		void applyRotation(CXMMATRIX transform);
		void applyRotation(const XMFLOAT4X4 & transform);

		const XMFLOAT3 &  right()const;
		 XMVECTOR rightVector() const;
		 const XMFLOAT3 & direction()const;
		 XMVECTOR  directionVector()const;		
		void  setPosition(const XMFLOAT3 & position);
		void  setPosition(FLOAT x, FLOAT y, FLOAT z);
		void  setPosition(FXMVECTOR position);
		const XMFLOAT3 & position()const;
		 XMVECTOR  positionVector()const;
		const XMFLOAT3 & up()const;
		 XMVECTOR  upVector()const;

		bool & displayWireFrame();
		
		virtual void initialize()override;
		virtual void update(const GameTimer & gameTimer)override;
		virtual void draw(const GameTimer & gameTimer)override;


	private:
		ProxyModel(const ProxyModel & rhs);
		ProxyModel & operator=(const ProxyModel & rhs);

		XMFLOAT3 mDirection;
		bool mDisplayWireFrame;
		Effect * mEffect;
		ID3D11Buffer * mIndexBuffer;
		ID3D11Buffer * mVertexBuffer;
		UINT mIndexCount;
		BasicMaterial  * mMaterial;
		std::string mModelFileName;
		XMFLOAT3 mPosition;
		XMFLOAT3 mRight;
		XMFLOAT3 mUp;

		XMFLOAT4X4 mScaleMatrix;
		XMFLOAT4X4 mWorldMatrix;
	
	};
}
