#include "ProxyModel.h"
#include <Exceptions\GameException.h>
#include <Effects\Pass.h>
#include <Misc\RasterizerStates.h>
#include <Misc\VectorHelper.h>
#include <Misc\MatrixHelper.h>
#include <3DObjects\Model.h>
#include <3DObjects\Mesh.h>
#include <Misc\MatrixHelper.h>

namespace Library
{


	RTTI_DEFINITIONS(ProxyModel)

		ProxyModel::ProxyModel(Game& game, Camera& camera, const std::string& modelFileName, float scale)
		: DrawableGameComponent(game, camera),
		mModelFileName(modelFileName), mEffect(nullptr), mMaterial(nullptr),
		mVertexBuffer(nullptr), mIndexBuffer(nullptr), mIndexCount(0),
		mWorldMatrix(MatrixHelper::INDENTITY), mScaleMatrix(MatrixHelper::INDENTITY), mDisplayWireFrame(true),
		mPosition(VectorHelper::ZERO), mDirection(VectorHelper::FORWARD), mUp(VectorHelper::UP), mRight(VectorHelper::RIGHT)
	{
			XMStoreFloat4x4(&mScaleMatrix, XMMatrixScaling(scale, scale, scale));
		}

	ProxyModel::~ProxyModel()
	{
		DeleteObject(mMaterial);
		DeleteObject(mEffect);
		ReleaseObject(mVertexBuffer);
		ReleaseObject(mIndexBuffer);
	}

	const XMFLOAT3& ProxyModel::position() const
	{
		return mPosition;
	}

	const XMFLOAT3& ProxyModel::direction() const
	{
		return mDirection;
	}

	const XMFLOAT3& ProxyModel::up() const
	{
		return mUp;
	}

	const XMFLOAT3& ProxyModel::right() const
	{
		return mRight;
	}

	XMVECTOR ProxyModel::positionVector() const
	{
		return XMLoadFloat3(&mPosition);
	}

	XMVECTOR ProxyModel::directionVector() const
	{
		return XMLoadFloat3(&mDirection);
	}

	XMVECTOR ProxyModel::upVector() const
	{
		return XMLoadFloat3(&mUp);
	}

	XMVECTOR ProxyModel::rightVector() const
	{
		return XMLoadFloat3(&mRight);
	}

	bool& ProxyModel::displayWireFrame()
	{
		return mDisplayWireFrame;
	}

	void ProxyModel::setPosition(FLOAT x, FLOAT y, FLOAT z)
	{
		XMVECTOR position = XMVectorSet(x, y, z, 1.0f);
		setPosition(position);
	}

	void ProxyModel::setPosition(FXMVECTOR position)
	{
		XMStoreFloat3(&mPosition, position);
	}

	void ProxyModel::setPosition(const XMFLOAT3& position)
	{
		mPosition = position;
	}

	void ProxyModel::applyRotation(CXMMATRIX transform)
	{
		XMVECTOR direction = XMLoadFloat3(&mDirection);
		XMVECTOR up = XMLoadFloat3(&mUp);

		direction = XMVector3TransformNormal(direction, transform);
		direction = XMVector3Normalize(direction);

		up = XMVector3TransformNormal(up, transform);
		up = XMVector3Normalize(up);

		XMVECTOR right = XMVector3Cross(direction, up);
		up = XMVector3Cross(right, direction);

		XMStoreFloat3(&mDirection, direction);
		XMStoreFloat3(&mUp, up);
		XMStoreFloat3(&mRight, right);
	}

	void ProxyModel::applyRotation(const XMFLOAT4X4 & transform)
	{
		XMMATRIX transformMatrix = XMLoadFloat4x4(&transform);
		applyRotation(transformMatrix);
	}

	void ProxyModel::initialize()
	{
		SetCurrentDirectory(Utility::ExecutableDirectory().c_str());

		std::unique_ptr<Model> model(new Model(*mGame, mModelFileName, true));

		mEffect = new Effect(*mGame);
		mEffect->LoadCompiledEffect(L"BasicEffect.cso");

		mMaterial = new BasicMaterial();
		mMaterial->initialize(mEffect);

		Mesh* mesh = model->meshes().at(0);
		mMaterial->createVertexBuffer(mGame->device(), *mesh, &mVertexBuffer);
		mesh->createIndexBuffer(&mIndexBuffer);
		mIndexCount = mesh->indices().size();
	}

	void ProxyModel::update(const GameTimer& gameTime)
	{
		XMMATRIX worldMatrix = XMMatrixIdentity();
		MatrixHelper::setForward(worldMatrix, mDirection);
		MatrixHelper::setUp(worldMatrix, mUp);
		MatrixHelper::setRight(worldMatrix, mRight);
		MatrixHelper::setTranslation(worldMatrix, mPosition);

		XMStoreFloat4x4(&mWorldMatrix, XMLoadFloat4x4(&mScaleMatrix) * worldMatrix);
	}

	void ProxyModel::draw(const GameTimer & gametime)
	{
		ID3D11DeviceContext* direct3DDeviceContext = mGame->deviceContext();
		direct3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Pass* pass = mMaterial->currentTechnique()->passes().at(0);
		ID3D11InputLayout* inputLayout = mMaterial->inputLayouts().at(pass);
		direct3DDeviceContext->IASetInputLayout(inputLayout);

		UINT stride = mMaterial->vertexSize();
		UINT offset = 0;
		direct3DDeviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
		direct3DDeviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX wvp = XMLoadFloat4x4(&mWorldMatrix) * mCamera->viewMatrix() * mCamera->projectionMatrix();
		mMaterial->WorldViewProjection() << wvp;

		pass->apply(0, direct3DDeviceContext);

		if (mDisplayWireFrame)
		{
			mGame->deviceContext()->RSSetState(RasterizerStates::wireFrame);
			direct3DDeviceContext->DrawIndexed(mIndexCount, 0, 0);
			mGame->deviceContext()->RSSetState(nullptr);
		}
		else
		{
			direct3DDeviceContext->DrawIndexed(mIndexCount, 0, 0);
		}
	}


	
}