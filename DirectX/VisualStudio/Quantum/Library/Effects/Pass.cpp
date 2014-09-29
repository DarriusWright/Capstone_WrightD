#include "Pass.h"
#include <Window\Game.h>
#include <Effects\Technique.h>


namespace Library
{

	Pass::Pass(Game & game, Technique & technique, ID3DX11EffectPass * pass) : mGame(game), mTechnique(technique), mPass(pass)
	{
		mPass->GetDesc(&mPassDesc);
		mName = mPassDesc.Name;
	}


	Pass::~Pass()
	{
		
	}


	const std::string & Pass::name()const
	{
		return mName;
	}

	Technique & Pass::getTechnique()
	{
		return mTechnique;
	}
	ID3DX11EffectPass * Pass::getPass()const
	{
		return mPass;
	}
	const D3DX11_PASS_DESC & Pass::passDesc()const
	{
		return mPassDesc;
	}
	void Pass::createInputLayout(const D3D11_INPUT_ELEMENT_DESC * inputElementDesc, UINT numElements, ID3D11InputLayout ** inputLayout)
	{
		HRESULT hr = mGame.device()->CreateInputLayout(inputElementDesc, numElements, mPassDesc.pIAInputSignature, mPassDesc.IAInputSignatureSize, inputLayout);

		if (FAILED(hr))
		{
			throw GameException("ID3D11Device::CreateInputLayout() failed", hr);
		}
	}
	void Pass::apply(UINT flags, ID3D11DeviceContext  * deviceContext)
	{
		mPass->Apply(flags, deviceContext);
	}

}
