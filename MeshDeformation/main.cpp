#include "global.h"
#include "MeshLoader.h"
#include "ShaderLoader.h"
#include "TextureLoader.h"
#include "InputController.h"

// The MAIN function, from here we start the application and run the game loop
int main()
{
	//Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);// anti-aliasing
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Mesh", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//Load Mesh
	MeshLoader* meshLoader = new MeshLoader();
	meshLoader->LoadObj("Rabbit.obj");
	auto vertices = meshLoader->GetVertices();
	auto uvs = meshLoader->GetUVs();
	//Load Shader
	GLuint programID = ShaderLoader::LoadShader("vertex.shader", "fragment.shader");
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	//Load Texture
	GLuint Texture = TextureLoader::LoadDDS("Rabbit.dds");
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
	//Load
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
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint matrix_location = glGetUniformLocation(programID, "MVP");
	GLuint model_matrix_location = glGetUniformLocation(programID, "M");
	GLuint view_matrix_location = glGetUniformLocation(programID, "V");
	//Render
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);

		InputController::ComputeMatricesFromInputs(window);
		mat4 ProjectionMatrix = InputController::GetProjectionMatrix();
		mat4 ViewMatrix = InputController::GetViewMatrix();
		mat4 ModelMatrix = mat4(1.0);
		mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE, &ViewMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	//Cleanup
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &TextureID);

	glfwTerminate();
	return 0;
}