#include <Misc\Common.h>
#include <Window\Game.h>

namespace Library
{

	struct InputElementInfo
	{
		
		InputElementInfo( const char * name, DXGI_FORMAT format, D3D11_INPUT_CLASSIFICATION perData) :
		mName(name), mFormat(format), mPerData(perData)
		{

		}

		const char * mName;
		DXGI_FORMAT mFormat;
		D3D11_INPUT_CLASSIFICATION mPerData;
	};

	struct InputInfo
	{

	public:

		InputInfo(Game & game, ID3DX11EffectPass * pass) : mGame(&game), mPass(pass)
		{

		}

		inline void addInput(InputElementInfo input)
		{
			mInputElements.push_back(input);
			mInputElementDescs.push_back({ input.mName, 0, input.mFormat, 0, D3D11_APPEND_ALIGNED_ELEMENT, input.mPerData, 0 });

		}
		inline void addInput(D3D11_INPUT_ELEMENT_DESC input)
		{
			mInputElementDescs.push_back(input);
			mInputElements.push_back(InputElementInfo(input.SemanticName,input.Format,input.InputSlotClass));
		}

		inline void createInput()
		{
			D3DX11_PASS_DESC passDesc;
			mPass->GetDesc(&passDesc);
			HRESULT hr;
			if (FAILED(hr = mGame->device()->CreateInputLayout(&mInputElementDescs[0], mInputElementDescs.size() * sizeof(D3D11_INPUT_ELEMENT_DESC), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mInputLayout)))
			{
				throw GameException(L"ID3D11Device::CreateInputLayout() failed", hr);
			}

		}

		ID3D11InputLayout * mInputLayout;

		std::vector<InputElementInfo> mInputElements;

	private:
		std::vector<D3D11_INPUT_ELEMENT_DESC> mInputElementDescs;
		ID3DX11EffectPass * mPass;
		Game * mGame;
	};
}