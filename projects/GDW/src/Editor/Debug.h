#pragma once

#include <iostream>
#include <string>

#include "ConsoleColors.h"

using namespace std;

namespace OMG
{
	class Debug
	{
	public:
		static void Log(string message);
		static void LogWarning(string message);
		static void LogError(string message);
		static void LogSuccess(string message);

	private:
		Debug() {};
		Debug(Debug const&) = delete;
		void operator=(Debug const&) = delete;
		
	};
}