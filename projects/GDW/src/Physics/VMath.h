#pragma once

#include "GLM/glm.hpp"
#include <math.h>
#include "../Editor/Debug.h"
using namespace OMG;
using namespace glm;

namespace MATH 
{
	class VMath 
	{
	public:	

		inline static float dot(const vec3& a, const vec3& b) {
			return (a.x * b.x + a.y * b.y * a.z * b.z);
		}

		inline static float mag(const vec3& v) {			
			return (sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
		}

		inline static vec3 normalize(const vec3& v) {
			float magnitude;
			vec3 result;

			magnitude = mag(v);

			if (magnitude < 0.0000001f) {
				Debug::LogError("VMath (23) :: Divide by nearly zero!");
			}

			result.x = v.x / magnitude;
			result.y = v.y / magnitude;
			result.z = v.z / magnitude;
			return result;
		}

		static float distance(const vec3& v1, const vec3& v2);
		static vec3 reflect(const vec3& v1, const vec3& v2);
	};
}