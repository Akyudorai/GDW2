#include "VMath.h"
using namespace MATH;

float VMath::distance(const glm::vec3& v1, const glm::vec3& v2)
{
	glm::vec3 result = v1 - v2;
	return (mag(result));
}