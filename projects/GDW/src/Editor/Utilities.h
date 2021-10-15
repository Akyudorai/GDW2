#pragma once

#include <list>
using namespace std;

namespace OMG
{
	class Utilities
	{
	public:
		template<typename T>
		static T* GetAtIndex(list<T*> _list, int _index);
	};
}