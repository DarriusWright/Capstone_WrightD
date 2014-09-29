#include "RayTraceCamera.h"

namespace Library
{
	RTTI_DEFINITIONS(RayTraceCamera);
	RayTraceCamera::RayTraceCamera(Game & game) : Camera(game)
	{
	}


	RayTraceCamera::~RayTraceCamera()
	{
	}

	void RayTraceCamera::computeUVW()
	{
		//camera direction 
		XMStoreFloat3(&mW, (eyeVector() - XMLoadFloat3(&mLookAt)));
		XMStoreFloat3(&mW, XMVector3Normalize(XMLoadFloat3(&mW)));

		//up crossed with 
		XMStoreFloat3(&mU, XMVector3Cross(XMLoadFloat3(&mUp), XMLoadFloat3(&mW)));
		XMStoreFloat3(&mU, XMVector3Normalize(XMLoadFloat3(&mU)));
		
		XMStoreFloat3(&mV, XMVector3Cross(XMLoadFloat3(&mW), XMLoadFloat3(&mU)));
	}


	void RayTraceCamera::initialize()
	{
		Camera::initialize();
		computeUVW();
	}

	void RayTraceCamera::update(const GameTimer & timer)
	{
		computeUVW();
	}

	XMFLOAT3 RayTraceCamera::eye(){ return position(); }
	XMVECTOR RayTraceCamera::eyeVector(){ return positionVector(); }
	void RayTraceCamera::setEyeVector(float x, float y, float z) { setPosition(x, y, z); }
	void RayTraceCamera::setEyeVector(XMFLOAT3 eye){ setPosition(eye); }
	void RayTraceCamera::setEyeVector(XMVECTOR eye){ setPosition(eye); }

	XMFLOAT3 RayTraceCamera::u()const{ return mU; }
	XMFLOAT3 RayTraceCamera::v()const{ return mV; }
	XMFLOAT3 RayTraceCamera::w()const{ return mW; }
	XMFLOAT3 RayTraceCamera::lookAt()const{ return mLookAt; }

	XMVECTOR RayTraceCamera::uVector(){ return XMLoadFloat3(&mU); }
	XMVECTOR RayTraceCamera::vVector(){ return XMLoadFloat3(&mV); }
	XMVECTOR RayTraceCamera::wVector(){ return XMLoadFloat3(&mW); }
	XMVECTOR RayTraceCamera::lookAtVector(){ return XMLoadFloat3(&mLookAt); }

	void RayTraceCamera::setLookAt(float x, float y, float z){ mLookAt = XMFLOAT3(x, y, z); }
	void RayTraceCamera::setLookAt(XMFLOAT3 lookAtPoint){ mLookAt = lookAtPoint; }
	void RayTraceCamera::setLookAt(XMVECTOR lookAtPoint){ XMStoreFloat3(&mLookAt, lookAtPoint); }
}