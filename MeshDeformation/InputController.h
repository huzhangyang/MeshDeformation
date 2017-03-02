#pragma once

#include "global.h"
#include <glm/gtc/matrix_transform.hpp>

class InputController {
public:
	static void ComputeMatricesFromInputs(GLFWwindow* window);
	static mat4 GetViewMatrix();
	static mat4 GetProjectionMatrix();
private:
};
