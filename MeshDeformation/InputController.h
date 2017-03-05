#pragma once

#include "global.h"
#include <glm/gtc/matrix_transform.hpp>

class InputController {
public:
	static void InitMouseCallback(GLFWwindow* window);
	static void ComputeMatricesFromInputs(GLFWwindow* window);
	static mat4 GetMVP();
	static vec3 convertTo3DCoordinate(double mouseX, double mouseY);
private:
};
