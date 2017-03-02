#include "MeshLoader.h"

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
				temp_vertices.push_back(vertex);
			}
			else if (header.compare("vt") == 0)
			{
				vec2 uv;
				file >> uv.x >> uv.y;
				temp_uvs.push_back(uv);
			}
			else if (header.compare("vn") == 0)
			{
				vec3 normal;
				file >> normal.x >> normal.y >> normal.z;
				temp_normals.push_back(normal);
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

		for (unsigned int i = 0; i<vertexIndices.size(); i++)
		{
			unsigned int vertexIndex = vertexIndices[i];
			unsigned int uvIndex = uvIndices[i];
			unsigned int normalIndex = normalIndices[i];

			vec3 vertex = temp_vertices[vertexIndex - 1];
			vec2 uv = temp_uvs[uvIndex - 1];
			vec3 normal = temp_normals[normalIndex - 1];

			out_vertices.push_back(vertex);
			out_uvs.push_back(uv);
			out_normals.push_back(normal);
		}
	}
	else
	{
		cout << "File Open Failed." << endl;
		return;
	}
}

vector<vec3> MeshLoader::GetVertices()
{
	return out_vertices;
}

vector<vec2> MeshLoader::GetUVs()
{
	return out_uvs;
}

vector<vec3> MeshLoader::GetNormals()
{
	return out_normals;
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