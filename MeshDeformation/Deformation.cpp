#include "Deformation.h"

static vector<vec3> controlPoints;
static int currentCPIndex = -1;

static vector<vec3> vertices;
static vector<int> vertexIndices;
static vector<int[4]> edgeNeighbors;//each vector stores the four neighbors (vi, vj, vl, vr) of an edge(ek = vi->vj)

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

void Deformation::Deform()
{
	InitData();
	DeformStepOne();
	DeformStepTwo();
}

void Deformation::InitData()
{
	vertices = MeshLoader::GetVertices();
	vertexIndices = MeshLoader::GetVertexIndices();

	CalculateEdgeNeighbors();
}

void Deformation::DeformStepOne()
{
	int w = 1000;
	MatrixXf A = MatrixXf::Zero((vertexIndices.size() / 3 + 1) * 6, vertices.size() * 2);
	VectorXf B = VectorXf::Zero((vertexIndices.size() / 3 + 1) * 6);
	MatrixXf Edge(2, 8);
	MatrixXf E(2, 2);

	Edge << -1, 0, 1, 0, 0, 0, 0, 0, 0, -1, 0, 1, 0, 0, 0, 0;

	for (int i = 0; i < vertexIndices.size(); i++)
	{
		int vi = edgeNeighbors[i][0];
		int vj = edgeNeighbors[i][1];
		int vl = edgeNeighbors[i][2];
		int vr = edgeNeighbors[i][3];

		float ex = vertices[vj].x - vertices[vi].x;
		float ey = vertices[vj].y - vertices[vi].y;
		E << ex, ey, ey, -ex;

		int matrixSize = vr == -1 ? 6 : 8;// if no vr, matrixLength is 6 instead of 8
		MatrixXf G(matrixSize, 4);
		MatrixXf H(2, matrixSize);

		for (int k = 0; k < matrixSize / 2; k++)
		{
			G(2 * k, 0) = vertices[edgeNeighbors[i][k]].x;
			G(2 * k, 1) = vertices[edgeNeighbors[i][k]].y;
			G(2 * k, 2) = 1;
			G(2 * k, 3) = 0;
			G(2 * k + 1, 0) = vertices[edgeNeighbors[i][k]].y;
			G(2 * k + 1, 1) = -1 * vertices[edgeNeighbors[i][k]].x;
			G(2 * k + 1, 2) = 0;
			G(2 * k + 1, 3) = 1;
		}

		if (vr == -1)
		{
			H = Edge.block<2, 6>(0, 0) - E * ((G.transpose() * G).inverse()) * G.transpose();
		}
		else
		{
			H = Edge - E * ((G.transpose() * G).inverse()) * G.transpose();
		}

		for (int k = 0; k < matrixSize / 2; k++)
		{
			A(2 * i, 2 * edgeNeighbors[i][k]) = H(0, 2 * k);
			A(2 * i, 2 * edgeNeighbors[i][k] + 1) = H(0, 2 * k + 1);
			A(2 * i + 1, 2 * edgeNeighbors[i][k]) = H(1, 2 * k);
			A(2 * i + 1, 2 * edgeNeighbors[i][k] + 1) = H(1, 2 * k + 1);
		}
	}

	for (int i = 0; i < controlPoints.size(); i++)
	{
		B(vertexIndices.size() * 2 + 2 * i) = w * controlPoints[i].x;
		B(vertexIndices.size() * 2 + 2 * i + 1) = w * controlPoints[i].y; 
		A(vertexIndices.size() * 2 + 2 * i, 2 * i) = (float)w;
		A(vertexIndices.size() * 2 + 2 * i + 1, 2 * i + 1) = (float)w;
	}
	//solve
	VectorXf newVertices = (A.transpose() * A).llt().solve(A.transpose()*B);
	//assign newVertices
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i].x = newVertices[2 * i];
		vertices[i].y = newVertices[2 * i + 1];
	}
}

void Deformation::DeformStepTwo()
{
}

void Deformation::CalculateEdgeNeighbors()
{
	edgeNeighbors = vector<int[4]>(vertexIndices.size());
	int vi, vj, vl, vr;

	for (int i = 0; i < vertexIndices.size(); i++)
	{
		if (i % 3 == 0)
		{
			vi = vertexIndices[i];
			vj = vertexIndices[i + 1];
			vl = vertexIndices[i + 2];
		}
		else if (i % 3 == 1)
		{
			vi = vertexIndices[i];
			vj = vertexIndices[i + 1];
			vl = vertexIndices[i - 1];
		}
		else if (i % 3 == 2)
		{
			vi = vertexIndices[i];
			vj = vertexIndices[i - 2];
			vl = vertexIndices[i - 1];
		}
		//search all the other triangles for vr(possibly no vr)
		vr = -1;
		for (int k = 0; k < vertexIndices.size(); k += 3)
		{
			if (k == i - i % 3) //in the same triangle
			{
				continue;
			}
			//search if vi and vj in the triangle
			if (vertexIndices[k] == vi || vertexIndices[k + 1] == vi || vertexIndices[k + 2] == vi)
			{
				if (vertexIndices[k] == vj || vertexIndices[k + 1] == vj || vertexIndices[k + 2] == vj)
				{
					vr = vertexIndices[k] + vertexIndices[k + 1] + vertexIndices[k + 2] - vi - vj;//the other vertex is vr
					break;
				}
			}
		}
		edgeNeighbors[i][0] = vi;
		edgeNeighbors[i][1] = vj;
		edgeNeighbors[i][2] = vl;
		edgeNeighbors[i][3] = vr;
	}
}
