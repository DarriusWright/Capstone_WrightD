#pragma once

#include <Misc\Common.h>

namespace Library
{

	class Game;
	class Effect;
	class Pass;
	class Technique
	{

	public:
		Technique(Game & game, Effect & effect, ID3DX11EffectTechnique * technique );
		virtual ~Technique();

		Effect & getEffect();
		const std::string & name() const;
		ID3DX11EffectTechnique * getTechnique();
		void setTechnique(ID3DX11EffectTechnique * techique);
		const std::vector<Pass*> & passes()const;
		const std::map<std::string,Pass*> & passesByName()const;

	private:

		Technique(const Technique & rhs);
		Technique & operator=(const Technique & rhs);

		Effect & mEffect;
		Game & mGame;
		ID3DX11EffectTechnique * mTechnique;
		D3DX11_TECHNIQUE_DESC mTechniqueDesc;
		std::string mName;
		std::vector<Pass*> mPasses;
		std::map<std::string, Pass*> mPassesByName;
	};

}