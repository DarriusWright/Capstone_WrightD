#pragma once
#include <string>
#include <Windows.h>
#include <exception>
class GameException : public std::exception
{
public:
	GameException(std::wstring message, HRESULT result);
	GameException(std::wstring message);
	GameException(std::string message, HRESULT result);
	GameException(std::string message);

	~GameException();
	std::wstring whatw();
protected:
	std::wstring mMessage;
	HRESULT mResult;
};

