#include "Effect.h"
#include <d3dcompiler.h>
#include <Exceptions/GameException.h>
#include <Window\Game.h>
namespace Library
{
	Effect::Effect(Game & game) : mGame(game), mEffect(nullptr)
	{

	}
	Effect::~Effect()
	{
		//ReleaseObject(mEffect);
		for (Technique * technique : mTechniques)
		{
			DeleteObject(technique);
		}

		for (Variable * variable : mVariables)
		{
			DeleteObject(variable)
		}
	}

	 void Effect::CompileEffectFromFile(ID3D11Device * device, ID3DX11Effect ** effect, const std::wstring & filename)
	 {

		 UINT shaderFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
		 shaderFlags |= D3DCOMPILE_DEBUG;
		 shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		 ID3D10Blob * compiledShader = nullptr;
		 ID3D10Blob * errorMessages = nullptr;
		 HRESULT hr = D3DCompileFromFile(filename.c_str(), nullptr, nullptr, nullptr, "fx_5_0", shaderFlags, 0, &compiledShader, &errorMessages);

		 if (FAILED(hr))
		 {
			 throw GameException(L"D3DX11CompileFromFile() failed.", hr);
		 }

		 hr = D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, device, effect);

		 if (FAILED(hr))
		 {
			 throw GameException(L"D3DX11CreateEffectFromMemory() failed", hr);
		 }

		 ReleaseObject(compiledShader);
	 }
	 void Effect::LoadCompiledEffect(ID3D11Device * device, ID3DX11Effect ** effect, const std::wstring & filename)
	 {
		 std::vector<char> compiledShader;
		 Utility::LoadBinaryFile(filename, compiledShader);
		 HRESULT hr = D3DX11CreateEffectFromMemory(&compiledShader.front(),compiledShader.size(), NULL, device,effect);
		 if (FAILED(hr))
		 {
			 throw GameException("D3DX11CreateEffectFromMemory failed.", hr);
		 }

	 }
	Game & Effect::getGame(){ return mGame; }
	ID3DX11Effect * Effect::getEffect()const{ return mEffect; }
	void Effect::setEffect(ID3DX11Effect * effect){ mEffect = effect; }
	const D3DX11_EFFECT_DESC & Effect::effectDesc()const{ return mEffectDesc; }
	const std::vector<Technique*> & Effect::techniques()const{ return mTechniques; }
	const std::map<std::string, Technique*> & Effect::techniquesByName()const{ return mTechniquesByName; }

	const std::vector<Variable*>& Effect::variables()const{ return mVariables; }
	const std::map<std::string, Variable*> & Effect::variablesByName()const{ return mVariablesByName; }
	void Effect::CompileFromFile(const std::wstring & fileName)
	{
		UINT shaderFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		ID3D10Blob * compiledShader = nullptr;
		ID3D10Blob * errorMessages = nullptr;
		HRESULT hr = D3DCompileFromFile(fileName.c_str(), nullptr, nullptr, nullptr, "fx_5_0", shaderFlags, 0, &compiledShader, &errorMessages);

		if (FAILED(hr))
		{
			throw GameException(L"D3DX11CompileFromFile() failed.", hr);
		}

		hr = D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, mGame.device(), &mEffect);

		if (FAILED(hr))
		{
			throw GameException(L"D3DX11CreateEffectFromMemory() failed", hr);
		}

		ReleaseObject(compiledShader);

		//mTechnique = mEffect->GetTechniqueByName("main11");

		//if (mTechnique == nullptr)
		//{
		//	throw GameException(L"ID3D11Effect::GetTechniqueByName() unable to find techique main11.", hr);
		//}

		//mPass = mTechnique->GetPassByName("p0");
		//if (mPass == nullptr)
		//{
		//	throw GameException(L"ID3D11EffectTechnique::GetPassByName() unable to find pass p0", hr);
		//}

		initialize();
	}
	void Effect::LoadCompiledEffect(const std::wstring & fileName)
	{
		std::vector<char> compiledShader;
		Utility::LoadBinaryFile(fileName, compiledShader);
		HRESULT hr = D3DX11CreateEffectFromMemory(&compiledShader.front(), compiledShader.size(), NULL, mGame.device(), &mEffect);
		if (FAILED(hr))
		{
			throw GameException("D3DX11CreateEffectFromMemory failed.", hr);
		}
		initialize();
	}


	void Effect::initialize()
	{
		HRESULT hr = mEffect->GetDesc(&mEffectDesc);

		if (FAILED(hr))
		{
			throw GameException("ID3DX11Effect::GetDesc() failed.", hr);
		}

		for (UINT i = 0; i < mEffectDesc.Techniques; i++)
		{
			Technique * technique = new Technique(mGame, *this, mEffect->GetTechniqueByIndex(i));
			mTechniques.push_back(technique);
			mTechniquesByName.insert(std::pair<std::string, Technique*>(technique->name(), technique));
		}

		for (UINT i = 0; i < mEffectDesc.GlobalVariables; i++)
		{
			Variable * variable = new Variable(*this, mEffect->GetVariableByIndex(i));
			mVariables.push_back(variable);
			mVariablesByName.insert(std::pair<std::string, Variable*>(variable->name(), variable));
		}
	}
}