#include "Variable.h"
#include <Exceptions\GameException.h>

namespace Library
{

	Variable::Variable(Effect & effect, ID3DX11EffectVariable * variable) : mEffect(effect), mVariable(variable)
	{
		mVariable->GetDesc(&mVariableDesc);
		mName = mVariableDesc.Name;
	}


	Variable::~Variable()
	{
		
	}



	const std::string & Variable::name()const
	{
		return mName;
	}

	ID3DX11EffectVariable *  Variable::getVariable()const{ return mVariable; }
	const D3DX11_EFFECT_VARIABLE_DESC &  Variable::variableDesc()const{ return mVariableDesc; }
	const D3DX11_EFFECT_TYPE_DESC &  Variable::typeDesc()const{ return mTypeDesc; }
	ID3DX11EffectType *  Variable::type()const{ return mType; }
	void  Variable::setVariable(ID3DX11EffectVariable * variable){ mVariable = variable; }


	Variable & Variable::operator<< (CXMMATRIX value)
	{
		ID3DX11EffectMatrixVariable * variable = mVariable->AsMatrix();
		if (!variable->IsValid())
		{
			throw GameException("Invalid effect variable cast");
		}

		variable->SetMatrix(reinterpret_cast<const float *>(&value));
		return *this;
	}

	Variable & Variable::operator<< (ID3D11UnorderedAccessView * value)
	{
		ID3DX11EffectUnorderedAccessViewVariable * variable = mVariable->AsUnorderedAccessView();

		if (!variable->IsValid())
		{
			throw GameException("Invalid effect variable cast");
		}

		variable->SetUnorderedAccessView(value);
		return *this;
	}


	Variable & Variable::operator<< (ID3D11ShaderResourceView * value)
	{
		ID3DX11EffectShaderResourceVariable * variable = mVariable->AsShaderResource();
		if (!variable->IsValid())
		{
			throw GameException("Invalid effect variable cast");
		}

		variable->SetResource(value);
		return *this;
	}
	Variable & Variable::operator << (FXMVECTOR value)
	{
		ID3DX11EffectVectorVariable * variable = mVariable->AsVector();
		if (!variable->IsValid())
		{
			throw GameException("Invalid effect variable cast");
		}

		variable->SetFloatVector(reinterpret_cast<const float *>(&value));
		return *this;
	}
	Variable & Variable::operator<< (float value)
	{
		ID3DX11EffectScalarVariable * variable = mVariable->AsScalar();
		if (!variable->IsValid())
		{
			throw GameException("Invalid effect variable cast");
		}

		variable->SetFloat(value);
		return *this;
	}

}