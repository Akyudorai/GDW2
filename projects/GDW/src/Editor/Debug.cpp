#include "Debug.h"

namespace OMG
{
	void Debug::Log(string message)
	{
		cout <<  message << endl;
	}

	void Debug::LogWarning(string message)
	{
		cout << KYEL << message << RST << endl;
	}

	void Debug::LogError(string message)
	{
		cout << KRED << message << RST << endl;
	}

	void Debug::LogSuccess(string message)
	{
		cout << KGRN << message << RST << endl;
	}
}