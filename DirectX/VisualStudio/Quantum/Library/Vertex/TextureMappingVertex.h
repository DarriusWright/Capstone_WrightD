#include <Misc\Common.h>

namespace Library
{


	typedef struct _TextureMappingVertex
	{
		XMFLOAT4 mPosition;
		XMFLOAT2 mTextureCoordinates;;

		_TextureMappingVertex()
		{

		}
		_TextureMappingVertex(XMFLOAT4 position, XMFLOAT2 textCoords) : mPosition(position), mTextureCoordinates(textCoords)
		{

		}

	}TextureMappingVertex;

}