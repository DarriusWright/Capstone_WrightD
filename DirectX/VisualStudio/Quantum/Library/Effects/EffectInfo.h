#include <Misc\Common.h>
#include <d3dcompiler.h>
#include <Exceptions/GameException.h>
#include <Window\Game.h>
namespace Library
{
	struct EffectInfo
	{
		EffectInfo(Game & game, const char * path, const char * techniqueName, const char * passName) : mPath(path), mTechniqueName(techniqueName), mPassName(passName), mGame(&game), mEffect(nullptr), mTechnique(nullptr), mPass(nullptr)
		{
			UINT shaderFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
			shaderFlags |= D3DCOMPILE_DEBUG;
			shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

			ID3D10Blob * compiledShader = nullptr;
			ID3D10Blob * errorMessages = nullptr;
			HRESULT hr = D3DCompileFromFile(L"Content\\Effects\\BinaryAlpha.fx", nullptr, nullptr, nullptr, "fx_5_0", shaderFlags, 0, &compiledShader, &errorMessages);

			if (FAILED(hr))
			{
				throw GameException(L"D3DX11CompileFromFile() failed.", hr);
			}

			hr = D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, mGame->device(), &mEffect);

			if (FAILED(hr))
			{
				throw GameException(L"D3DX11CreateEffectFromMemory() failed", hr);
			}

			ReleaseObject(compiledShader);

			mTechnique = mEffect->GetTechniqueByName("main11");

			if (mTechnique == nullptr)
			{
				throw GameException(L"ID3D11Effect::GetTechniqueByName() unable to find techique main11.", hr);
			}

			mPass = mTechnique->GetPassByName("p0");
			if (mPass == nullptr)
			{
				throw GameException(L"ID3D11EffectTechnique::GetPassByName() unable to find pass p0", hr);
			}

		}

		~EffectInfo()
		{
			ReleaseObject(mPass);
			ReleaseObject(mTechnique);
			ReleaseObject(mEffect);
		}

		ID3DX11Effect * mEffect;
		ID3DX11EffectTechnique * mTechnique;
		ID3DX11EffectPass * mPass;

	private:
		const char * mPath;
		const char * mTechniqueName;
		const char * mPassName;



		Game * mGame;

	};
}