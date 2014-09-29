#pragma once

#include <Misc\Common.h>

namespace Library
{

	class Effect;
	class Variable
	{
	public:
		Variable(Effect & effect , ID3DX11EffectVariable * variable);
		~Variable();
		const std::string & name()const;
		ID3DX11EffectVariable * getVariable()const;
		const D3DX11_EFFECT_VARIABLE_DESC & variableDesc()const;
		const D3DX11_EFFECT_TYPE_DESC & typeDesc()const;
		ID3DX11EffectType * type()const;
		void setVariable(ID3DX11EffectVariable * variable);

		Variable & operator<< (ID3D11UnorderedAccessView * value);
		Variable & operator<< (CXMMATRIX value);
		Variable & operator<< (ID3D11ShaderResourceView * value);
		Variable & operator << (FXMVECTOR value);
		Variable & operator<< (float value);

	private:
		Variable(const Variable & rhs);
		Variable & operator=(const Variable & rhs);
		std::string mName;
		ID3DX11EffectVariable * mVariable;
		Effect & mEffect;
		D3DX11_EFFECT_VARIABLE_DESC mVariableDesc;
		ID3DX11EffectType * mType;
		D3DX11_EFFECT_TYPE_DESC mTypeDesc;


	};

}