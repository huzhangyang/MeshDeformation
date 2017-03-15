#pragma once

#include "global.h"

class MeshLoader {
public:
	static void LoadObj(string filename);
	static void LoadObj2D(string filename);
	static vector<vec3> GetVertices();
	static vector<vec2> GetUVs();
	static vector<vec3> GetNormals();
	static vector<int> GetVertexIndices();
private:
	static vector<string> split(string& s, const char* delim);
};