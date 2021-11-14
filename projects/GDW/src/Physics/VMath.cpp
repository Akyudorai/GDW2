#include "VMath.h"
using namespace MATH;

float VMath::distance(const glm::vec3& v1, const glm::vec3& v2)
{
	vec3 result = v1 - v2;
	return (mag(result));
}

vec3 VMath::reflect(const vec3& v1, const vec3& v2)
{
	float d = dot(v1, v2);
	vec3 result = (v1 - (2.0f * d) * v2);
	return result;
}