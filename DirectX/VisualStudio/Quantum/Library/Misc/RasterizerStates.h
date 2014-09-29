#pragma once

#include <Misc\Common.h>


namespace Library
{
	class RasterizerStates
	{
	public:

		static ID3D11RasterizerState * backCulling;
		static ID3D11RasterizerState * frontCulling;
		static ID3D11RasterizerState * disableCulling;
		static ID3D11RasterizerState * wireFrame;

		static void initialize(ID3D11Device * device);
		static void release();

		~RasterizerStates();

	private:
		RasterizerStates();
		RasterizerStates(const RasterizerStates & rhs);
		RasterizerStates& operator= (const RasterizerStates & rhs);
	};
}