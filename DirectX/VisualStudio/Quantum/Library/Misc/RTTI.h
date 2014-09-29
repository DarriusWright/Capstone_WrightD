#pragma once

#include <string>

namespace Library
{
	class RTTI
	{
	public:
		virtual const unsigned int & typeIdInstance()const = 0;
		virtual RTTI * queryInterface(const unsigned int id)const
		{
			return nullptr;
		}

		virtual bool is(const unsigned int id)const
		{
			return false;
		}

		virtual bool is(const std::string & name)const
		{
			return false;
		}

		template<typename T> T* as()const
		{
			if (is(T::typeIdClass()))
			{
				return (T*)this;
			}

			return nullptr;
		}
	};

#define RTTI_DECLARATIONS(Type, ParentType)							  					\
public:																  					\
	typedef ParentType Parent;										  					\
	static std::string typeName(){ return std::string(#Type); }		  					\
	virtual const unsigned int & typeIdInstance() const { return Type::typeIdClass(); }	\
	static const unsigned int & typeIdClass(){ return sRunTimeTypeId; }					\
	virtual Library::RTTI * queryInterface(const unsigned int id)						\
	{																					\
	if (id == sRunTimeTypeId)															\
	{																					\
	return (RTTI*)this;																	\
	}																					\
		else																			\
	{																					\
	return Parent::queryInterface(id);													\
		}																				\
	}																					\
	virtual bool is(const unsigned int id)const											\
	{																					\
		if (id == sRunTimeTypeId)														\
		{																				\
			return true;																\
		}																				\
		else																			\
		{																				\
			return Parent::is(id);														\
		}																				\
	}																					\
																						\
	virtual bool is(const std::string & name)const										\
	{																					\
		if (name == typeName())															\
		{																				\
			return true;																\
		}																				\
		else																			\
		{																				\
			return Parent::is(name);													\
		}																				\
	}																					\
																						\
private:																				\
	static unsigned int sRunTimeTypeId;															  
																								  
#define RTTI_DEFINITIONS(Type)	unsigned int Type::sRunTimeTypeId = (unsigned int)& Type::sRunTimeTypeId;					  
}																								  