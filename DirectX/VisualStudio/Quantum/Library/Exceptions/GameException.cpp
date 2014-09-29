#include "GameException.h"


GameException::GameException(std::wstring message) : mMessage(message)
{
}

GameException::GameException(std::wstring message, HRESULT result) : mMessage(message), mResult(result)
{
}

GameException::GameException(std::string message) 
{
	wchar_t * exceptionString = new wchar_t[100];
	const char * messageChar = message.c_str();
	mbstowcs(exceptionString, messageChar, message.size() );
	mMessage = std::wstring(exceptionString);
	delete[] exceptionString;
}

GameException::GameException(std::string message, HRESULT result) : mResult(result)
{
	wchar_t * exceptionString;
	const char * messageChar = message.c_str();
	mbstowcs(exceptionString, messageChar, message.size());
	mMessage = std::wstring(exceptionString);
}



GameException::~GameException()
{
}

std::wstring GameException::whatw()
{
	return mMessage;
}
