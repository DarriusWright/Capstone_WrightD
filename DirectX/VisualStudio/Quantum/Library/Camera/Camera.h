#pragma once
#include <Misc\Common.h>
#include <Exceptions\GameException.h>
#include <Window\Game.h>
namespace Library
{
	class GameTimer;
	class Camera : public GameComponent
	{
		RTTI_DECLARATIONS(Camera, GameComponent);

	public:
		Camera(Game & game);
		Camera(Game & game, float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance);
		virtual ~Camera();

		const XMFLOAT3& position()const;
		const XMFLOAT3& direction()const;
		const XMFLOAT3& up()const;
		const XMFLOAT3& right();

		XMVECTOR positionVector()const;
		XMVECTOR directionVector()const;
		XMVECTOR upVector()const;
		XMVECTOR rightVector()const;

		float aspectRatio()const;
		float fieldOfView()const;
		float nearPlaneDistance()const;
		float farPlaneDistance()const;

		XMMATRIX viewMatrix()const;
		XMMATRIX projectionMatrix()const;
		XMMATRIX viewProjectionMatrix()const;

		virtual void setPosition(FLOAT x, FLOAT y, FLOAT z); // does FLOAT change depending on the platform
		virtual void setPosition(FXMVECTOR position);
		virtual void setPosition(const XMFLOAT3 & position);
		virtual void reset();
		virtual void initialize()override;
		virtual void update(const GameTimer  & gameTime)override;
		virtual void updateViewMatrix();
		virtual void updateProjectionMatrix();
		void applyRotation(CXMMATRIX transform);
		void applyRotation(const XMFLOAT4X4 & transform);
		static const float DEFAULT_FIELD_OF_VIEW;
		static const float DEFAULT_NEAR_PLANE_DISTANCE;
		static const float DEFAULT_FAR_PLANE_DISTANCE;
	protected:
		float mFieldOfView;
		float mAspectRatio;
		float mNearPlaneDistance;
		float mFarPlaneDistance;

		XMFLOAT3 mPosition;
		XMFLOAT3 mDirection;
		XMFLOAT3 mUp;
		XMFLOAT3 mRight;

		XMFLOAT4X4 mViewMatrix;
		XMFLOAT4X4 mProjectionMatrix;

	private:
		Camera(const Camera & rhs);
		Camera& operator=(const Camera & rhs);

	};

}