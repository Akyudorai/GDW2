#pragma once

#include "GLM/glm.hpp"
#include <math.h>
#include "../Editor/Debug.h"
using namespace OMG;

namespace MATH 
{
	class VMath 
	{
	public:	
		inline static float mag(const glm::vec3& v) {			
			return (sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
		}

		inline static glm::vec3 normalize(const glm::vec3& v) {
			float magnitude;
			glm::vec3 result;

			magnitude = mag(v);

			if (magnitude < 0.0000001f) {
				Debug::LogError("VMath (23) :: Divide by nearly zero!");
			}

			result.x = v.x / magnitude;
			result.y = v.y / magnitude;
			result.z = v.z / magnitude;
			return result;
		}

		static float distance(const glm::vec3& v1, const glm::vec3& v2);
	};
}