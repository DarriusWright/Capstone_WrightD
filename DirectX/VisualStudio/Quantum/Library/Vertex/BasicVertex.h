#include <Misc\Common.h>

namespace Library
{


	typedef struct _BasicEffectVertex
	{
		XMFLOAT4 mPosition;
		XMFLOAT4 mColor;

		_BasicEffectVertex()
		{

		}
		_BasicEffectVertex(XMFLOAT4 position, XMFLOAT4 color) : mPosition(position), mColor(color)
		{

		}

	}BasicEffectVertex;

}