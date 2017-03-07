#include "global.h"
#include "MeshLoader.h"
#include "ShaderLoader.h"
#include "TextureLoader.h"
#include "InputController.h"
#include "Deformation.h"

// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mesh", NULL, NULL);
	glfwMakeContextCurrent(window);
	InputController::InitMouseCallback(window);

	// Init GLEW
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Load Mesh
	MeshLoader* meshLoader = new MeshLoader();
	meshLoader->LoadObj("Rabbit.obj");
	auto vertices = meshLoader->GetVertices();
	auto uvs = meshLoader->GetUVs();

	// Load Shader
	GLuint programID = ShaderLoader::LoadShader("vertex.shader", "fragment.shader");
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Load Texture
	GLuint TextureID = TextureLoader::LoadDDS("Rabbit.dds");

	// Load into a VBO
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while(!glfwWindowShouldClose(window)) 
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);

		// Set Camera
		InputController::ComputeMatricesFromInputs(window);
		mat4 MVP = InputController::GetMVP();
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		auto controlPoints = Deformation::GetControlPoints();
		if (controlPoints.size() > 0)
		{
			/*GLuint cpbuffer;
			glGenBuffers(1, &cpbuffer);
			glBindBuffer(GL_ARRAY_BUFFER, cpbuffer);
			glBufferData(GL_ARRAY_BUFFER, controlPoints.size() * sizeof(vec3), &controlPoints[0], GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glDrawArrays(GL_POINTS, 0, controlPoints.size());
			glDisableVertexAttribArray(2);
			glDeleteBuffers(1, &cpbuffer);*/
		}

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &TextureID);

	glfwTerminate();
	return 0;
}