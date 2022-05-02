#pragma once

#include "Vector.h"

struct Transform
{
	Float3 pos;
	Float3 scale = Float3(1, 1, 1);
	Float3 rotation;
};

