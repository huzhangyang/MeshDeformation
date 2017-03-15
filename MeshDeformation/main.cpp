#include "global.h"
#include "MeshLoader.h"
#include "ShaderLoader.h"
#include "TextureLoader.h"
#include "InputController.h"
#include "Deformation.h"

int main()
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mesh Deformation", NULL, NULL);
	glfwMakeContextCurrent(window);
	InputController::InitMouseCallback(window);

	// Init GLEW
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPointSize(8.0f);

	// Load Mesh
	MeshLoader::LoadObj2D("man.obj");
	auto vertices = MeshLoader::GetVertices();
	//auto uvs = meshLoader->GetUVs();

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

	/*GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);*/

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while(!glfwWindowShouldClose(window)) 
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);

		// Set Camera
		InputController::ComputeMatricesFromInputs(window);
		mat4 MVP = InputController::GetMVP();
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// Draw the mesh
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		//Visualize control points
		auto controlPoints = Deformation::GetControlPoints();
		glUseProgram(0);
		glColor3f(0.0f, 0.0f, 1.0f);
		glBegin(GL_POINTS);
		for (int i = 0; i < controlPoints.size(); i++)
		{
			vec4 cp = MVP * vec4(controlPoints[i], 1);
			glVertex3f(cp.x, cp.y, cp.z);
		}
		glEnd();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup
	glDeleteBuffers(1, &vertexbuffer);
	//glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &TextureID);
	glfwTerminate();
	return 0;
}