#pragma once

#include "global.h"

class MeshLoader {
public:
	static void LoadObj(string filename);
	static void LoadObj2D(string filename);
	static void OverrideVertices(vector<vec3> vertices);
	static void ResetVertices();
	static void SaveCurrentMesh();
	static vector<vec3>* GetSequencedVertices();
	static vector<vec3>* GetVertices();
	static vector<vec2>* GetUVs();
	static vector<vec3>* GetNormals();
	static vector<int>* GetVertexIndices();
private:
	static vector<string> split(string& s, const char* delim);
};