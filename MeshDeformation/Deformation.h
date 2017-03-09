#pragma once

#include "global.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Eigen/Dense"

class Deformation {
public:
	static void AddControlPoint(vec3 point);
	static void ClearControlPoints();
	static vector<vec3> GetControlPoints();
	static bool FindNearbyControlPoints(vec3 pos, float searchDis);
	static void MoveControlPoint(vec3 newPos);
private:
};
