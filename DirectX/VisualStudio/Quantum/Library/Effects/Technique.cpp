#include "Technique.h"
#include <Effects\Pass.h>

namespace Library
{


	Technique::Technique(Game & game, Effect & effect, ID3DX11EffectTechnique * technique) : mGame(game), mEffect(effect), mTechnique(technique)
	{
		mTechnique->GetDesc(&mTechniqueDesc);
		mName = mTechniqueDesc.Name;
		for (UINT i = 0; i < mTechniqueDesc.Passes; i++)
		{
			Pass * pass = new Pass(game, *this, mTechnique->GetPassByIndex(i));
			mPasses.push_back(pass);
			mPassesByName.insert(std::pair<std::string, Pass*>(pass->name(),pass));
		}
	}


	Technique::~Technique()
	{
		ReleaseObject(mTechnique);

		for (Pass * pass : mPasses)
		{
			DeleteObject(pass);
		}
	}

	const std::string & Technique::name()const
	{
		return mName;
	}
	Effect & Technique::getEffect()
	{
		return mEffect;
	}

	void Technique::setTechnique(ID3DX11EffectTechnique * techique)
	{
		mTechnique = techique;
	}
	const std::vector<Pass*> & Technique::passes()const
	{
		return mPasses;
	}
	const std::map<std::string, Pass*> & Technique::passesByName()const
	{
		return mPassesByName;
	}
}