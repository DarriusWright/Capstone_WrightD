#pragma once
#include <Camera\Camera.h>

namespace Library
{

	class RayTraceCamera : public Camera
	{
		RTTI_DECLARATIONS(RayTraceCamera, Camera);
	
	public:
		RayTraceCamera(Game & game);
		~RayTraceCamera();
		XMFLOAT3 u()const;
		XMFLOAT3 v()const;
		XMFLOAT3 w()const;
		XMFLOAT3 lookAt()const;

		XMVECTOR uVector();
		XMVECTOR vVector();
		XMVECTOR wVector();
		XMVECTOR lookAtVector();

		void setLookAt(float x, float y, float z);
		void setLookAt(XMFLOAT3 lookAtPoint);
		void setLookAt(XMVECTOR lookAtPoint);

		void computeUVW();

		XMFLOAT3 eye();
		XMVECTOR eyeVector();
		void setEyeVector(float x, float y, float z);
		void setEyeVector(XMFLOAT3 eye);
		void setEyeVector(XMVECTOR eye);

		void initialize()override;
		void update(const GameTimer & timer)override;

	protected:
		XMFLOAT3 mLookAt;
		XMFLOAT3 mU;
		XMFLOAT3 mV;
		XMFLOAT3 mW;
		float mExposureTime;
	};

	/*
	get the difference from the eye to the look at vector 
	negate to get the camrea direction 
	*/
}
