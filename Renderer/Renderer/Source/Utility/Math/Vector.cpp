#include "Vector.h"

#include <cmath>

Float3 Vector::Normalize(const Float3& vec)
{
	float length = std::hypot(vec.x, vec.y, vec.z);

	return Float3(vec.x / length, vec.y / length, vec.z / length);
}
