#pragma once

#include "global.h"

class MeshLoader {
public:
	void LoadObj(string filename);
	vector<vec3> GetVertices();
	vector<vec2> GetUVs();
	vector<vec3> GetNormals();
private:
	vector<int> vertexIndices, uvIndices, normalIndices;
	vector<vec3> temp_vertices;
	vector<vec2> temp_uvs;
	vector<vec3> temp_normals;
	vector<vec3> out_vertices;
	vector<vec2> out_uvs;
	vector<vec3> out_normals;

	vector<string> split(string& s, const char* delim);
};