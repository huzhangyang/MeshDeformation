#include "Deformation.h"

static vector<vec3> controlPoints;
static int currentCPIndex = -1;

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

bool Deformation::FindNearbyControlPoints(vec3 pos, float searchDis)
{
	bool ret = false;
	float minDis = SCREEN_WIDTH;
	for (int i = 0; i < controlPoints.size(); i++)
	{
		vec3 cp = controlPoints[i];
		float dis = sqrt(pow(cp.x - pos.x, 2) + pow(cp.y - pos.y, 2) + pow(cp.z - pos.z, 2));
		if (dis < minDis && dis < searchDis)
		{
			ret = true;
			minDis = dis;
			currentCPIndex = i;
		}
	}
	return ret;
}

void Deformation::MoveControlPoint(vec3 newPos)
{
	if (currentCPIndex >= 0)
	{
		controlPoints[currentCPIndex] = newPos;
		currentCPIndex = -1;
	}
}