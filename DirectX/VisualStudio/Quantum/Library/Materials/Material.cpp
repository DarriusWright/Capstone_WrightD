#include "Material.h"
#include <3DObjects\Model.h>
#include <Exceptions\GameException.h>
#include <Effects\Pass.h>
namespace Library
{
	RTTI_DEFINITIONS(Material);
	Material::Material() : mEffect(nullptr), mCurrentTechnique(nullptr), mDefaultTechniqueName(), mInputLayouts()
	{
	}

	Material::Material(const std::string & defaultTechniqueName) : mEffect(nullptr), mCurrentTechnique(nullptr), mDefaultTechniqueName(defaultTechniqueName), mInputLayouts()
	{

	}


	Material::~Material()
	{
		for (std::pair<Pass*, ID3D11InputLayout*> inputLayout : mInputLayouts)
	
		{
			ReleaseObject(inputLayout.second);
		}


	}

	Variable * Material::operator[](const std::string & variableName)
	{
		const std::map<std::string, Variable*> & variables = mEffect->variablesByName();
		Variable * foundVariable = nullptr;

		std::map<std::string, Variable *>::const_iterator found = variables.find(variableName);

		if (found != variables.end())
		{
			foundVariable = found->second;
		}

		return foundVariable;
	}


	Effect * Material::getEffect()const
	{
		return mEffect;
	}

	Technique * Material::currentTechnique()const
	{
		return mCurrentTechnique;
	}
	void Material::setCurrentTechnique(Technique * currentTechnique)
	{
		mCurrentTechnique = currentTechnique;
	}

	const std::map<Pass *, ID3D11InputLayout * > & Material::inputLayouts()const
	{
		return mInputLayouts;
	}

	void Material::initialize(Effect * effect)
	{
		mEffect = effect;
		assert(mEffect != nullptr);

		Technique * defaultTechnique = nullptr;

		assert(mEffect->techniques().size() > 0);
		if (!mDefaultTechniqueName.empty())
		{
			defaultTechnique = mEffect->techniquesByName().at(mDefaultTechniqueName);
			assert(defaultTechnique != nullptr);

		}
		else
		{
			defaultTechnique = mEffect->techniques().at(0);
		}

		setCurrentTechnique(defaultTechnique);

	}


	void Material:: createVertexBuffer(ID3D11Device * device, const Model& model, std::vector<ID3D11Buffer*> & vertexBuffers)const
	{
		vertexBuffers.reserve(model.meshes().size());
		for (Mesh * mesh : model.meshes())
		{
			ID3D11Buffer * vertexBuffer;
			createVertexBuffer(device, *mesh, &vertexBuffer);
			vertexBuffers.push_back(vertexBuffer);
		}
	}

	void Material::createInputLayout(const std::string & techinqueName, const std::string & passName, D3D11_INPUT_ELEMENT_DESC * inputElementDescriptions, UINT inputElementDescriptionCount)
	{
		Technique * technique = mEffect->techniquesByName().at(techinqueName);
		assert(technique != nullptr);
		Pass * pass = technique->passesByName().at(passName);
		assert(pass != nullptr);

		ID3D11InputLayout * inputLayout;
		pass->createInputLayout(inputElementDescriptions, inputElementDescriptionCount, &inputLayout);
		mInputLayouts.insert ( std::pair<Pass*, ID3D11InputLayout * >(pass, inputLayout));
	}
}