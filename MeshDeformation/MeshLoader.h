#pragma once

#include "global.h"

class MeshLoader {
public:
	void LoadObj(string filename);
private:
	vector<int> vertexIndices, uvIndices, normalIndices;
	vector<vec3> temp_vertices;
	vector<vec2> temp_uvs;
	vector<vec3> temp_normals;

	static vector<string> split(string& s, const char* delim);
};