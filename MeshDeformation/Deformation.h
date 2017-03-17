#pragma once

#include "global.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Eigen/Dense"
#include "MeshLoader.h"

using namespace Eigen;

class Deformation {
public:
	static void AddControlPoint(vec3 point);
	static void ClearControlPoints();
	static vector<vec3> GetControlPoints();
	static bool FindNearbyControlPoints(vec3 pos, float searchDis);
	static void MoveControlPoint(vec3 newPos);
	static void Deform();
	static void InitData();
private:
	static void CalculateEdgeNeighbors();
	static int GetNearestVertexIndex(vec3 point);
};
