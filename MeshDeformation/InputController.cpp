#include "InputController.h"

vec3 position = vec3(1, 0.5, 0);
float horizontalAngle = 4.71f;
float verticalAngle = 0;
float fov = 90.0f;
float speed = 3.0f;

static mat4 ViewMatrix;
static mat4 ProjectionMatrix;

void mouseCallback(GLFWwindow* window, int button, int action, int mods) 
{
	if (button == GLFW_MOUSE_BUTTON_LEFT &&  action == GLFW_RELEASE) 
	{
		//set a control point
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		cout << mouseX << ' ' << mouseY << endl;
		vec3 pos = InputController::convertTo3DCoordinate(mouseX, mouseY);
		cout << pos.x << ' ' << pos.y << ' ' << pos.z << endl;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT &&  action == GLFW_RELEASE)
	{
		//clear latest control point
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE &&  action == GLFW_RELEASE)
	{
		//clear all control points
	}
}

void InputController::InitMouseCallback(GLFWwindow * window)
{
	glfwSetMouseButtonCallback(window, mouseCallback);
}

void InputController::ComputeMatricesFromInputs(GLFWwindow* window)
{
	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Right vector
	vec3 right = vec3(sin(horizontalAngle - 3.14f / 2.0f), 0, cos(horizontalAngle - 3.14f / 2.0f));

	// Up vector
	glm::vec3 up = glm::cross(right, direction);

	// Move up
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		verticalAngle += deltaTime * speed / 5;
	}
	// Move down
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		verticalAngle -= deltaTime * speed / 5;
	}
	// Move left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		horizontalAngle += deltaTime * speed / 5;
	}
	// Move right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		horizontalAngle -= deltaTime * speed / 5;
	}
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		position -= right * deltaTime * speed;
	}
	// Reset
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		position = vec3(1, 0.5, 0);
		horizontalAngle = 4.71f;
		verticalAngle = 0;
	}

	float FoV = fov;// - 5 * glfwGetMouseWheel();
	ProjectionMatrix = glm::perspective(FoV, SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
	ViewMatrix = glm::lookAt(position, position + direction, up);

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

mat4 InputController::GetMVP()
{
	mat4 ModelMatrix = mat4(1.0);
	return ProjectionMatrix * ViewMatrix * ModelMatrix;
}

vec3 InputController::convertTo3DCoordinate(double mouseX, double mouseY)
{
	double modelview[16], projection[16];
	int viewport[4];
	double winX, winY, winZ;
	double object_x = 0, object_y = 0, object_z = 0;
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	winX = mouseX;
	winY = (float)viewport[3] - mouseY;
	glReadPixels((GLint)mouseX, (GLint)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
	gluUnProject((GLdouble)winX, (GLdouble)winY, (GLdouble)winZ, modelview, projection, viewport, &object_x, &object_y, &object_z);
	return vec3(object_x, object_y, object_z);
}
