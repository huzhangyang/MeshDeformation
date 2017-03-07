#include "Deformation.h"

static vector<vec3> controlPoints;

void Deformation::AddControlPoint(vec3 point)
{
	controlPoints.push_back(point);
}

void Deformation::ClearControlPoints()
{
	controlPoints.clear();
}

vector<vec3> Deformation::GetControlPoints()
{
	return controlPoints;
}
