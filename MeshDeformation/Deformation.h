#pragma once

#include "global.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Eigen/Dense"
#include "MeshLoader.h"

using namespace Eigen;

struct Barycentric
{
	float w1 = 0.0f;
	int v1 = 0;
	float w2 = 0.0f;
	int v2 = 0;
	float w3 = 0.0f;
	int v3 = 0;
};

class Deformation {
public:
	static void AddControlPoint(vec3 point);
	static void ClearControlPoints();
	static vector<vec3> GetControlPoints();
	static bool FindNearbyControlPoints(vec3 pos, float searchDis);
	static void MoveControlPoint(vec3 newPos);
	static void Deform();
	static void InitData();
	static void Reset();
private:
	static void CalculateEdgeNeighbors();
	static int GetNearestVertexIndex(vec3 point);
	static Barycentric GetBarycentricCoordinate(vec3 point);
};
