#include "MeshLoader.h"

static vector<int> vertexIndices, uvIndices, normalIndices;
static vector<vec3> rawVertices;
static vector<vec2> rawUVs;
static vector<vec3> rawNormals;
static vector<vec3> outVertices;
static vector<vec2> outUVs;
static vector<vec3> outNormals;

static vector<vec3> currentVertices;

void MeshLoader::LoadObj(string filename)
{
	ifstream file(filename);
	if (file.is_open())
	{
		while (!file.eof())
		{
			string header;
			file >> header;
			//cout << "Read from file: " << header << endl;

			if (header.compare("v") == 0) 
			{
				vec3 vertex;
				file >> vertex.x >> vertex.y >> vertex.z;
				rawVertices.push_back(vertex);
			}
			else if (header.compare("vt") == 0)
			{
				vec2 uv;
				file >> uv.x >> uv.y;
				uv.y = -uv.y; //need this?
				rawUVs.push_back(uv);
			}
			else if (header.compare("vn") == 0)
			{
				vec3 normal;
				file >> normal.x >> normal.y >> normal.z;
				rawNormals.push_back(normal);
			}
			else if (header.compare("f") == 0)
			{
				string s1, s2, s3;
				file >> s1 >> s2 >> s3;
				vector<string> arg1 = split(s1, "/");
				vector<string> arg2 = split(s2, "/");
				vector<string> arg3 = split(s3, "/");
				vertexIndices.push_back(stoi(arg1[0]));
				vertexIndices.push_back(stoi(arg2[0]));
				vertexIndices.push_back(stoi(arg3[0]));
				uvIndices.push_back(stoi(arg1[1]));
				uvIndices.push_back(stoi(arg2[1]));
				uvIndices.push_back(stoi(arg3[1]));
				normalIndices.push_back(stoi(arg1[2]));
				normalIndices.push_back(stoi(arg2[2]));
				normalIndices.push_back(stoi(arg3[2]));
			}
		}

		cout << "File Read Complete." << endl;

		for (unsigned int i = 0; i< vertexIndices.size(); i++)
		{
			unsigned int vertexIndex = vertexIndices[i];
			unsigned int uvIndex = uvIndices[i];
			unsigned int normalIndex = normalIndices[i];

			vec3 vertex = rawVertices[vertexIndex - 1];
			vec2 uv = rawUVs[uvIndex - 1];
			vec3 normal = rawNormals[normalIndex - 1];

			outVertices.push_back(vertex);
			outUVs.push_back(uv);
			outNormals.push_back(normal);
		}
	}
	else
	{
		cout << "File Open Failed." << endl;
		return;
	}
}

void MeshLoader::LoadObj2D(string filename)
{
	ifstream file(filename);
	if (file.is_open())
	{
		while (!file.eof())
		{
			string header;
			file >> header;
			//cout << "Read from file: " << header << endl;

			if (header.compare("v") == 0)
			{
				vec3 vertex;
				file >> vertex.x >> vertex.y >> vertex.z;
				rawVertices.push_back(vertex);
			}
			else if (header.compare("f") == 0)
			{
				int v1, v2, v3;
				file >> v1 >> v2 >> v3;
				vertexIndices.push_back(v1 - 1);
				vertexIndices.push_back(v2 - 1);
				vertexIndices.push_back(v3 - 1);
			}
		}

		ResetVertices();

		cout << "File Read Complete." << endl;
	}
	else
	{
		cout << "File Open Failed." << endl;
		return;
	}
}

void MeshLoader::OverrideVertices(vector<vec3> vertices)
{
	currentVertices.clear();
	outVertices.clear();
	for (int i = 0; i < vertices.size(); i++)
	{
		currentVertices.push_back(vertices[i]);
	}
	for (unsigned int i = 0; i< vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		vec3 vertex = currentVertices[vertexIndex];
		outVertices.push_back(vertex);
	}
}

void MeshLoader::ResetVertices()
{
	currentVertices.clear();
	outVertices.clear();
	for (int i = 0; i < rawVertices.size(); i++)
	{
		currentVertices.push_back(rawVertices[i]);
	}
	for (unsigned int i = 0; i< vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		vec3 vertex = currentVertices[vertexIndex];
		outVertices.push_back(vertex);
	}
}

vector<vec3>* MeshLoader::GetSequencedVertices()
{
	return &outVertices;
}

vector<vec3>* MeshLoader::GetVertices()
{
	return &currentVertices;
}

vector<vec2>* MeshLoader::GetUVs()
{
	return &outUVs;
}

vector<vec3>* MeshLoader::GetNormals()
{
	return &outNormals;
}

vector<int>* MeshLoader::GetVertexIndices()
{
	return &vertexIndices;
}

vector<string> MeshLoader::split(string& s, const char* delim)
{
	vector<string> ret;
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != string::npos)
	{
		ret.push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last>0)
	{
		ret.push_back(s.substr(last, index - last));
	}

	return ret;
}