#pragma once
#include <Misc\Common.h>
namespace Library
{


	class VectorHelper
	{
	public:
		static const XMFLOAT3 RIGHT;
		static const XMFLOAT3 UP;
		static const XMFLOAT3 ZERO;
		static const XMFLOAT3 FORWARD;
		static const XMFLOAT3 BACKWARD;



		~VectorHelper();

	private:
		VectorHelper();
	};

}