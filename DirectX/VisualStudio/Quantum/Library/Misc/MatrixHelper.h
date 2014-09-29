#pragma once
#include <Misc\Common.h>
namespace Library
{

	class MatrixHelper
	{
	public:

		static const XMFLOAT4X4 INDENTITY;
		static const XMFLOAT4X4 ZERO;

		static void getForward(CXMMATRIX matrix, XMFLOAT3 & vector);
		static void getUp(CXMMATRIX matrix, XMFLOAT3 & vector);
		static void getRight(CXMMATRIX matrix, XMFLOAT3 & vector);
		static void getTranslation(CXMMATRIX matrix, XMFLOAT3 & vector);

		static void setForward(XMMATRIX & matrix, XMFLOAT3 & vector);
		static void setUp(XMMATRIX & matrix, XMFLOAT3 & vector);
		static void setRight(XMMATRIX & matrix, XMFLOAT3 & vector);
		static void setTranslation(XMMATRIX & matrix, XMFLOAT3 & vector);


		~MatrixHelper();

	private:
		MatrixHelper();
	};

}