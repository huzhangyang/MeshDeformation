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
	float w = 1000.0f;
	MatrixXf A1 = MatrixXf::Zero(2 * vertexIndices.size() + 2 * controlPoints.size(), 2 * vertices.size());
	VectorXf B1 = VectorXf::Zero(2 * vertexIndices.size() + 2 * controlPoints.size());
	MatrixXf A2 = MatrixXf::Zero(vertexIndices.size() + controlPoints.size(), vertices.size());
	VectorXf B2x = VectorXf::Zero(vertexIndices.size() + controlPoints.size());
	VectorXf B2y = VectorXf::Zero(vertexIndices.size() + controlPoints.size());
	MatrixXf Edge(2, 8);

	Edge << -1, 0, 1, 0, 0, 0, 0, 0,
		0, -1, 0, 1, 0, 0, 0, 0;
	//fill the lower half of matrix
	for (int i = 0; i < controlPoints.size(); i++)
	{
		Barycentric bc = GetBarycentricCoordinate(controlPoints[i]);
		A1(2 * vertexIndices.size() + 2 * i, 2 * bc.v1) = w * bc.w1;
		A1(2 * vertexIndices.size() + 2 * i, 2 * bc.v2) = w * bc.w2;
		A1(2 * vertexIndices.size() + 2 * i, 2 * bc.v3) = w * bc.w3;
		A1(2 * vertexIndices.size() + 2 * i + 1, 2 * bc.v1 + 1) = w * bc.w1;
		A1(2 * vertexIndices.size() + 2 * i + 1, 2 * bc.v2 + 1) = w * bc.w2;
		A1(2 * vertexIndices.size() + 2 * i + 1, 2 * bc.v3 + 1) = w * bc.w3;
		B1(2 * vertexIndices.size() + 2 * i) = w * controlPoints[i].x;
		B1(2 * vertexIndices.size() + 2 * i + 1) = w * controlPoints[i].y;
		A2(vertexIndices.size() + i, bc.v1) = w * bc.w1;
		A2(vertexIndices.size() + i, bc.v2) = w * bc.w2;
		A2(vertexIndices.size() + i, bc.v3) = w * bc.w3;
		B2x(vertexIndices.size() + i) = w * controlPoints[i].x;
		B2y(vertexIndices.size() + i) = w * controlPoints[i].y;
	}
	//Similarity Transformation
	for (int i = 0; i < vertexIndices.size(); i++)
	{
		int vi = edgeNeighbors[i][0];
		int vj = edgeNeighbors[i][1];
		int vl = edgeNeighbors[i][2];
		int vr = edgeNeighbors[i][3];

		float ex = vertices[vj].x - vertices[vi].x;
		float ey = vertices[vj].y - vertices[vi].y;
		Matrix2f E;
		E << ex, ey, ey, -ex;

		int matrixSize = vr == -1 ? 6 : 8;// if no vr, matrixLength is 6 instead of 8
		MatrixXf G(matrixSize, 4);
		MatrixXf H(2, matrixSize);

		for (int j = 0; j < matrixSize / 2; j++)
		{
			G(2 * j, 0) = vertices[edgeNeighbors[i][j]].x;
			G(2 * j, 1) = vertices[edgeNeighbors[i][j]].y;
			G(2 * j, 2) = 1;
			G(2 * j, 3) = 0;
			G(2 * j + 1, 0) = vertices[edgeNeighbors[i][j]].y;
			G(2 * j + 1, 1) = -vertices[edgeNeighbors[i][j]].x;
			G(2 * j + 1, 2) = 0;
			G(2 * j + 1, 3) = 1;
		}
		H = Edge.block(0, 0, 2, matrixSize) - E * ((((G.transpose() * G).inverse()) * G.transpose()).block(0, 0, 2 , matrixSize));

		for (int j = 0; j < matrixSize / 2; j++)
		{
			A1(2 * i, 2 * edgeNeighbors[i][j]) = H(0, 2 * j);
			A1(2 * i, 2 * edgeNeighbors[i][j] + 1) = H(0, 2 * j + 1);
			A1(2 * i + 1, 2 * edgeNeighbors[i][j]) = H(1, 2 * j);
			A1(2 * i + 1, 2 * edgeNeighbors[i][j] + 1) = H(1, 2 * j + 1);
		}
	}
	//solve part 1
	VectorXf newVertices = (A1.transpose() * A1).llt().solve(A1.transpose() * B1);
	//Scale Adjustment
	for (int i = 0; i < vertexIndices.size(); i++)
	{
		int vi = edgeNeighbors[i][0];
		int vj = edgeNeighbors[i][1];
		int vl = edgeNeighbors[i][2];
		int vr = edgeNeighbors[i][3];

		int matrixSize = vr == -1 ? 6 : 8;// if no vr, matrixLength is 6 instead of 8
		VectorXf V(matrixSize);
		MatrixXf G(matrixSize, 4);

		for (int j = 0; j < matrixSize / 2; j++)
		{
			V(2 * j) = newVertices(2 * edgeNeighbors[i][j]);
			V(2 * j + 1) = newVertices(2 * edgeNeighbors[i][j] + 1);
		}

		for (int j = 0; j < matrixSize / 2; j++)
		{
			G(2 * j, 0) = vertices[edgeNeighbors[i][j]].x;
			G(2 * j, 1) = vertices[edgeNeighbors[i][j]].y;
			G(2 * j, 2) = 1;
			G(2 * j, 3) = 0;
			G(2 * j + 1, 0) = vertices[edgeNeighbors[i][j]].y;
			G(2 * j + 1, 1) = -1 * vertices[edgeNeighbors[i][j]].x;
			G(2 * j + 1, 2) = 0;
			G(2 * j + 1, 3) = 1;
		}
		VectorXf t = ((((G.transpose() * G).inverse()) * G.transpose()).block(0, 0, 2, matrixSize)) * V;

		Matrix2f T;
		T << t(0, 0), t(1, 0), -t(1, 0), t(0, 0);
		Vector2f E;
		E << vertices[vj].x - vertices[vi].x, vertices[vj].y - vertices[vi].y;
		Vector2f TE = T * E / sqrt(t[0] * t[0] + t[1] * t[1]);
		B2x[i] = TE[0];
		B2y[i] = TE[1];
		A2(i, vi) = -1;
		A2(i, vj) = 1;
	}
	//solve part 2
	VectorXf newVerticesX = (A2.transpose() * A2).llt().solve(A2.transpose()*B2x);
	VectorXf newVerticesY = (A2.transpose() * A2).llt().solve(A2.transpose()*B2y);
	//assign newVertices
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i].x = newVerticesX[i];
		vertices[i].y = newVerticesY[i];
	}
	MeshLoader::OverrideVertices(vertices);
}

void Deformation::InitData()
{
	vertices = MeshLoader::GetVerticesList()[0];
	vertexIndices = MeshLoader::GetVertexIndices()[0];

	CalculateEdgeNeighbors();
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

int Deformation::GetNearestVertexIndex(vec3 point)
{
	int index = 0;
	float minDis = SCREEN_WIDTH * SCREEN_HEIGHT;
	for (int i = 0; i < vertices.size(); i++)
	{
		vec3 vertex = vertices[i];

		float sqrDis = pow(vertex.x - point.x, 2) + pow(vertex.y - point.y, 2) + pow(vertex.z - point.z, 2);
		if (sqrDis < minDis)
		{
			minDis = sqrDis;
			index = i;
		}
	}

	return index;
}

Barycentric Deformation::GetBarycentricCoordinate(vec3 point)
{
	Barycentric ret;
	for (int i = 0; i < vertexIndices.size(); i += 3)
	{
		vec3 A = vertices[vertexIndices[i]];
		vec3 B = vertices[vertexIndices[i + 1]];
		vec3 C = vertices[vertexIndices[i + 2]];

		vec3 v0 = B - A;
		vec3 v1 = C - A;
		vec3 v2 = point - A;

		float d00 = dot(v0, v0);
		float d01 = dot(v0, v1);
		float d11 = dot(v1, v1);
		float d20 = dot(v2, v0);
		float d21 = dot(v2, v1);
		float denom = d00 * d11 - d01 * d01;

		float w2 = (d11 * d20 - d01 * d21) / denom;
		float w3 = (d00 * d21 - d01 * d20) / denom;
		float w1 = 1.0f - w2 - w3;

		if (w1 >= 0 && w1 <= 1 && w2 >= 0 && w2 <= 1 && w3 >= 0 && w3 <= 1)
		{//point is inside the triangle(or on its edge or vertex)
			ret.w1 = w1;
			ret.w2 = w2;
			ret.w3 = w3;
			ret.v1 = vertexIndices[i];
			ret.v2 = vertexIndices[i + 1];
			ret.v3 = vertexIndices[i + 2];
			break;
		}

	}
	return ret;
}
