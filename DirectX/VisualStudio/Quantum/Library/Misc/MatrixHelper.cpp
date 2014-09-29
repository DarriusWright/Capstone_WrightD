#include "MatrixHelper.h"

namespace Library
{



	const XMFLOAT4X4 MatrixHelper::INDENTITY = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	const XMFLOAT4X4 MatrixHelper::ZERO = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	void MatrixHelper::getForward(CXMMATRIX matrix, XMFLOAT3 & vector)
	{
		XMFLOAT4 m3;
		XMStoreFloat4(&m3, matrix.r[2]);
		vector.x = -m3.x;
		vector.y = -m3.y;
		vector.z = -m3.z;
		
	}
	void MatrixHelper::getUp(CXMMATRIX matrix, XMFLOAT3 & vector)
	{
		XMFLOAT4 m2;
		XMStoreFloat4(&m2, matrix.r[1]);
		vector.x = m2.x;
		vector.y = m2.y;
		vector.z = m2.z;
	}
	void MatrixHelper::getRight(CXMMATRIX matrix, XMFLOAT3 & vector)
	{
		XMFLOAT4 m1;
		XMStoreFloat4(&m1, matrix.r[0]);
		vector.x = m1.x;
		vector.y = m1.y;
		vector.z = m1.z;
	}
	void MatrixHelper::getTranslation(CXMMATRIX matrix, XMFLOAT3 & vector)
	{
		XMFLOAT4 m3;
		XMStoreFloat4(&m3, matrix.r[3]);
		vector.x = m3.x;
		vector.y = m3.y;
		vector.z = m3.z;
	}
		 
	void MatrixHelper::setForward(XMMATRIX & matrix, XMFLOAT3 & vector)
	{
		XMFLOAT4 m3;
		XMStoreFloat4(&m3, matrix.r[2]);
		m3.x = -vector.x;
		m3.y = -vector.y;
		m3.z = -vector.z;

		matrix.r[2] = XMLoadFloat4(&m3);
	}
	void MatrixHelper::setUp(XMMATRIX & matrix, XMFLOAT3 & vector)
	{
		XMFLOAT4 m2;
		XMStoreFloat4(&m2, matrix.r[1]);
		m2.x = vector.x;
		m2.y = vector.y;
		m2.z = vector.z;

		matrix.r[1] = XMLoadFloat4(&m2);
	}
	void MatrixHelper::setRight(XMMATRIX & matrix, XMFLOAT3 & vector)
	{
		XMFLOAT4 m1;
		XMStoreFloat4(&m1, matrix.r[0]);
		m1.x = vector.x;
		m1.y = vector.y;
		m1.z = vector.z;

		matrix.r[0] = XMLoadFloat4(&m1);
	}
	void MatrixHelper::setTranslation(XMMATRIX & matrix, XMFLOAT3 & vector)
	{
		XMFLOAT4 m4;
		XMStoreFloat4(&m4, matrix.r[3]);
		m4.x = vector.x;
		m4.y = vector.y;
		m4.z = vector.z;

		matrix.r[3] = XMLoadFloat4(&m4);
	}

	MatrixHelper::MatrixHelper()
	{
	}


	MatrixHelper::~MatrixHelper()
	{
	}
}