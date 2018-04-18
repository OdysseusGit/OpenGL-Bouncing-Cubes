/*
 * A needlessly complicated OpenGL animation of rolling cubes.
 */

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Shader.h"
#include "Camera.h"
#include "Cube.h"

int main()
{
	//initialise GLFW
	if (!glfwInit())
	{
		std::cout << "Error: Failed to initialize GLFW." << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); //antialiasing x4
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //version 3.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//open a window and create its OpenGL context
	GLFWwindow* window;
	window = glfwCreateWindow(640, 480, "Playground", NULL, NULL);
	if (window == NULL) 
	{
		std::cout << "Error: Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); //bind the refreshrate
	
	//initialize GLEW
	if (glewInit() != GLEW_OK) 
	{
		std::cout << "Error: Failed to initialize GLEW." << std::endl;
		return -1;
	}

	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//cull the reverse faces
	glEnable(GL_CULL_FACE);

	//enable key presses
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	//position 3-vertices
	static const GLfloat posVertices[] = {
		-1.0f,	-1.0f,	-1.0f,
		-1.0f,	-1.0f,	 1.0f,
		-1.0f,	 1.0f,	 1.0f,
		 1.0f,	 1.0f,	-1.0f,
		-1.0f,	-1.0f,	-1.0f,
		-1.0f,	 1.0f,	-1.0f,
		 1.0f,	-1.0f,	 1.0f,
		-1.0f,	-1.0f,	-1.0f,
		 1.0f,	-1.0f,	-1.0f,
		 1.0f,	 1.0f,	-1.0f,
		 1.0f,	-1.0f,	-1.0f,
		-1.0f,	-1.0f,	-1.0f,
		-1.0f,	-1.0f,	-1.0f,
		-1.0f,	 1.0f,	 1.0f,
		-1.0f,	 1.0f,	-1.0f,
		 1.0f,	-1.0f,	 1.0f,
		-1.0f,	-1.0f,	 1.0f,
		-1.0f,	-1.0f,	-1.0f,
		-1.0f,	 1.0f,	 1.0f,
		-1.0f,	-1.0f,	 1.0f,
		 1.0f,	-1.0f,	 1.0f,
		 1.0f,	 1.0f,	 1.0f,
		 1.0f,	-1.0f,	-1.0f,
		 1.0f,	 1.0f,	-1.0f,
		 1.0f,	-1.0f,	-1.0f,
		 1.0f,	 1.0f,	 1.0f,
		 1.0f,	-1.0f,	 1.0f,
		 1.0f,	 1.0f,	 1.0f,
		 1.0f,	 1.0f,	-1.0f,
		-1.0f,	 1.0f,	-1.0f,
		 1.0f,	 1.0f,	 1.0f,
		-1.0f,	 1.0f,	-1.0f,
		-1.0f,	 1.0f,	 1.0f,
		 1.0f,	 1.0f,	 1.0f,
		-1.0f,	 1.0f,	 1.0f,
		 1.0f,	-1.0f,	 1.0f
	};

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer); //number of buffers, identifier vertexBuffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(posVertices), posVertices, GL_STATIC_DRAW); //pair the vertices with the buffer

	//load the shaders
	GLuint programID = LoadShaders("res/shaders/VertexShader.shader", "res/shaders/FragmentShader.shader");

	//get an identifier for the MVP matrix and colour vector
	GLuint mvpID = glGetUniformLocation(programID, "u_MVP");
	GLuint colourID = glGetUniformLocation(programID, "u_Colour");

	Cube cube[1000]; //hard code a maximum of 1000 cubes
	int cubeCount = 0;
	bool adder = true;

	//run until the escape key is pressed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
	{
		//clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//use our shader
		glUseProgram(programID);

		//scale the world to a 2D projection
		glm::mat4 projection = projectionMatrix(window, 640, 480); //perspective view
		glm::mat4 view = viewMatrix(window, 640, 480); //update the view using our inputs
		//glm::mat4 view = glm::lookAt(glm::vec3(0, -15, 60), glm::vec3(0, -15, 0), glm::vec3(0, 1, 0)); //static camera

		for (int i = 0; i < cubeCount; i++)
			cube[i].draw(projection, view, vertexBuffer, mvpID, colourID);

		//add successive cubes
		if (!glfwGetKey(window, GLFW_KEY_SPACE))
		{
			adder = true;
		}
		else if (adder == true && cubeCount < sizeof(cube) / sizeof(cube[0]))
		{
			cubeCount++;
			adder = false;
		}

		//swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//cleanup
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteProgram(programID);
	glfwTerminate();

	return 0;
}