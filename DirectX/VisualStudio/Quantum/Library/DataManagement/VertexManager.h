#pragma once

#include <DataManagement/BufferManager.h>
#include <Vertex\BasicVertex.h>
#include <Vertex\TextureMappingVertex.h>
namespace Library
{
	
	class VertexManager : public BufferManager
	{
	public:
		VertexManager(Game & game);
		~VertexManager();
		//template<class T>
		virtual void addMesh(Mesh * mesh);
		virtual UINT spaceLeft();
		virtual void initialize();

		static const UINT MAX_VERTICES = MEGABYTE / sizeof(TextureMappingVertex);
	private:
		UINT mVertexCount;
		
	};
}
