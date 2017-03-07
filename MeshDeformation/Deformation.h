#pragma once

#include "global.h"
#include <glm/gtc/matrix_transform.hpp>

class Deformation {
public:
	static void AddControlPoint(vec3 point);
	static void ClearControlPoints();
	static vector<vec3> GetControlPoints();
private:
};
