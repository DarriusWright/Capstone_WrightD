#pragma once

#include <Misc/Common.h>

namespace Library
{
	class Game;
	class RenderStateHelper
	{
	public:
		RenderStateHelper(Game & game);
		~RenderStateHelper();


		static void resetAll(ID3D11DeviceContext * deviceContext);
		ID3D11RasterizerState * rasterizerState();
		ID3D11BlendState * blendState();
		ID3D11DepthStencilState * depthStencilState();

		void saveRasterizerState();
		void restoreRasterizerState()const;

		void saveBlendState();
		void restoreBlendState()const;

		void saveDepthStencilState();
		void restoreDepthStencilState()const;

		void saveAll();
		void restoreAll()const;

	private:
		RenderStateHelper(const RenderStateHelper & rhs);
		RenderStateHelper& operator=(const RenderStateHelper & rhs);

		Game & mGame;
		ID3D11RasterizerState * mRasterizerState;
		ID3D11BlendState * mBlendState;
		FLOAT * mBlendFactor;
		UINT mSampleMask;
		ID3D11DepthStencilState * mDepthStencilState;
		UINT mStencilRef;

	};

}