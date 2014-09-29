#pragma once
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <d3d11_1.h>

using namespace DirectX;
using namespace DirectX::PackedVector;
namespace Library
{
	class Colors
	{
	public :
		static XMVECTORF32 RED ;
		static XMVECTORF32 BLUE ;
		static XMVECTORF32 GREEN ;
		static XMVECTORF32 PURPLE ;
		static XMVECTORF32 CYAN ;
		static XMVECTORF32 YELLOW ;
		static XMVECTORF32 WHITE ;
		static XMVECTORF32 BLACK ;
	};
}