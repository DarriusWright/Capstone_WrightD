#pragma once
#include <Misc\Common.h>
#include <Effects\Technique.h>
#include <Effects\Variable.h>

namespace Library
{

	class Game;
	class Effect
	{
	public:
		Effect(Game & game);
		virtual ~Effect();

		static void CompileEffectFromFile(ID3D11Device * device, ID3DX11Effect ** effect, const std::wstring & filename);
		static void LoadCompiledEffect(ID3D11Device * device, ID3DX11Effect ** effect, const std::wstring & filename);
		Game & getGame();
		ID3DX11Effect * getEffect()const;
		void setEffect(ID3DX11Effect * effect);
		const D3DX11_EFFECT_DESC & effectDesc()const;
		const std::vector<Technique*> & techniques()const;
		const std::map<std::string, Technique*> & techniquesByName()const;

		const std::vector<Variable*>& variables()const;
		const std::map<std::string, Variable*> & variablesByName()const;
		void CompileFromFile(const std::wstring & fileName);
		void LoadCompiledEffect(const std::wstring & fileName);


	private:
		Effect(const Effect & rhs);
		Effect& operator=(const Effect & rhs);

		void initialize();
		Game & mGame;
		ID3DX11Effect * mEffect;
		D3DX11_EFFECT_DESC mEffectDesc;
		std::vector<Technique*> mTechniques;
		std::map<std::string, Technique*> mTechniquesByName;
		std::vector<Variable*> mVariables;
		std::map<std::string, Variable*> mVariablesByName;
	};
}
