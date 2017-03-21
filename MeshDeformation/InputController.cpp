#include "InputController.h"

vec3 position = vec3(0, 0, 1);
float horizontalAngle = 3.14f;
float verticalAngle = 0;
float fov = 90.0f;
float speed = 3.0f;

static mat4 ViewMatrix;
static mat4 ProjectionMatrix;

bool movingCP = false;

void mouseCallback(GLFWwindow* window, int button, int action, int mods) 
{
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	vec3 world = InputController::convertToWorldCoordinate(mouseX, mouseY);

	if (button == GLFW_MOUSE_BUTTON_LEFT &&  action == GLFW_PRESS)
	{
		//select and move a control point
		if (!movingCP)
		{
			bool found = Deformation::FindNearbyControlPoints(world, 0.1f);
			if (found)
			{
				movingCP = true;
			}
		}
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT &&  action == GLFW_RELEASE) 
	{
		//place a control point(can't be too close to others)
		if (movingCP)
		{
			bool found = Deformation::FindNearbyControlPoints(world, 0.1f);
			if (!found)
			{
				Deformation::MoveControlPoint(world);
				Deformation::Deform();//core deformation function entry here
			}
			movingCP = false;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT &&  action == GLFW_RELEASE)
	{
		//set a control point(can't be too close to others)
		bool found = Deformation::FindNearbyControlPoints(world, 0.1f);
		if (!found)
		{
			Deformation::AddControlPoint(world);
		}
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE &&  action == GLFW_RELEASE)
	{
		//clear all control points
		Deformation::ClearControlPoints();
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
		//verticalAngle += deltaTime * speed / 5;
	}
	// Move down
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		//verticalAngle -= deltaTime * speed / 5;
	}
	// Move left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		//horizontalAngle += deltaTime * speed / 5;
	}
	// Move right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		//horizontalAngle -= deltaTime * speed / 5;
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
		position = vec3(0, 0, 1);
		horizontalAngle = 3.14f;
		verticalAngle = 0;
		Deformation::Reset();
	}
	// Draw Line
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	// Fill
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

vec3 InputController::convertToWorldCoordinate(double mouseX, double mouseY)
{
	vec4 translation = vec4(2 * mouseX / SCREEN_WIDTH - 1, -2 * mouseY / SCREEN_HEIGHT + 1, 0, 1);
	vec4 mappedTranslation = inverse(GetMVP()) * translation;
	return vec3(mappedTranslation.x, mappedTranslation.y, 0);
}
