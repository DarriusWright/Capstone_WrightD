#pragma once

#include <Misc/Common.h>

namespace Library
{

	class Game;
	class Technique;
	class Pass
	{
	public:
		Pass(Game & game, Technique & technique, ID3DX11EffectPass * pass);
		~Pass();
		const std::string & name()const;
		Technique & getTechnique();
		ID3DX11EffectPass * getPass()const;
		const D3DX11_PASS_DESC & passDesc()const;
		void createInputLayout(const D3D11_INPUT_ELEMENT_DESC * inputElementDesc, UINT numElements, ID3D11InputLayout ** inputLayout);
		void apply(UINT flags , ID3D11DeviceContext  * deviceContext);

	private:
		Game & mGame;
		std::string mName;
		Technique & mTechnique;
		ID3DX11EffectPass * mPass;
		D3DX11_PASS_DESC mPassDesc;
	};
}
