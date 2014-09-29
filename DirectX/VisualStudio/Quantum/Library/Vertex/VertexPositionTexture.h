#include <Misc\Common.h>
#pragma once

namespace Library
{

	typedef struct _VertexPositionTexture
	{
		XMFLOAT4 Position;
		XMFLOAT2 TextureCoordinates;

		_VertexPositionTexture() { }

		_VertexPositionTexture(const XMFLOAT4& position, const XMFLOAT2& textureCoordinates)
			: Position(position), TextureCoordinates(textureCoordinates) { }
	} VertexPositionTexture;

}