#pragma once

#include <Misc/Common.h>
namespace Library
{

	class Mesh;
	class Game;
	class BufferManager
	{
	public:
		BufferManager(Game & game,D3D11_BIND_FLAG flag);
		~BufferManager();
		virtual void addMesh(Mesh * mesh) = 0;
		virtual UINT spaceLeft() = 0;
		virtual void initialize();
		ID3D11Buffer * getBuffer() const;
		void setBuffer(ID3D11Buffer * buffer);
		static const UINT MEGABYTE = 1048576;

	protected:
		ID3D11Buffer * buffer;
		Game * mGame;
		D3D11_BIND_FLAG mBindFlag;

	};

}