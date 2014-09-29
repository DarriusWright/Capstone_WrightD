#pragma once

#include <DataManagement/BufferManager.h>
namespace Library
{
	class IndexManager : public BufferManager
	{
	public:
		IndexManager(Game & game);
		~IndexManager();

		virtual void addMesh(Mesh * mesh);
		virtual UINT spaceLeft();
		virtual void initialize();

		static const UINT MAX_INDICES = MEGABYTE / sizeof(UINT);
	private:
		//std::vector<UINT> mIndices;
		UINT mIndexCount;
	};
}
