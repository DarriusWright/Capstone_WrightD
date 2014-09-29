#include "RenderStateHelper.h"
#include <Window\Game.h>
namespace Library
{

	RenderStateHelper::RenderStateHelper(Game & game) : mGame(game),
		mRasterizerState(nullptr), mBlendFactor(new FLOAT[4]), mBlendState(nullptr),
		mSampleMask(UINT_MAX), mStencilRef(UINT_MAX), mDepthStencilState(nullptr)
	{
	}


	RenderStateHelper::~RenderStateHelper()
	{
		ReleaseObject(mRasterizerState);
		ReleaseObject(mBlendState);
		ReleaseObject(mDepthStencilState);
		DeleteObject(mBlendFactor);
	}

	void RenderStateHelper::resetAll(ID3D11DeviceContext * deviceContext)
	{
		deviceContext->RSSetState(nullptr);
		deviceContext->OMSetBlendState(nullptr,nullptr,UINT_MAX);
		deviceContext->OMSetDepthStencilState(nullptr, UINT_MAX);
	}


	ID3D11RasterizerState * RenderStateHelper::rasterizerState(){ return mRasterizerState; }
	ID3D11BlendState * RenderStateHelper::blendState(){ return mBlendState; }
	ID3D11DepthStencilState * RenderStateHelper::depthStencilState(){ return mDepthStencilState; }

	void RenderStateHelper::saveRasterizerState()
	{
		ReleaseObject(mRasterizerState);
		mGame.deviceContext()->RSGetState(&mRasterizerState);
	}
	void RenderStateHelper::restoreRasterizerState()const
	{
		mGame.deviceContext()->RSSetState(mRasterizerState);

	}

	void RenderStateHelper::saveBlendState()
	{
		ReleaseObject(mBlendState);
		mGame.deviceContext()->OMGetBlendState(&mBlendState, mBlendFactor, &mSampleMask);
	}
	void RenderStateHelper::restoreBlendState()const
	{
		mGame.deviceContext()->OMSetBlendState(mBlendState, mBlendFactor, mSampleMask);
	}

	void RenderStateHelper::saveDepthStencilState()
	{
		ReleaseObject(mDepthStencilState);
		mGame.deviceContext()->OMGetDepthStencilState(&mDepthStencilState, &mStencilRef);
	}
	void RenderStateHelper::restoreDepthStencilState()const
	{
		mGame.deviceContext()->OMSetDepthStencilState(mDepthStencilState, mStencilRef);
	}

	void RenderStateHelper::saveAll()
	{
		saveBlendState();
		saveDepthStencilState();
		saveRasterizerState();
	}
	void RenderStateHelper::restoreAll()const
	{
		restoreBlendState();
		restoreRasterizerState();
		restoreDepthStencilState();
	}

}