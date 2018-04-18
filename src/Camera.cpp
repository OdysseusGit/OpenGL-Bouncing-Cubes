#include <iostream>

#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Camera.h"

glm::vec3 position = glm::vec3(0, -15, 60); //initial position
float horizontalAngle = 3.14159f;
float verticalAngle = 0.0f;
float mouseSpeed = 0.005f;
float moveSpeed = 0.1f; //units per second

glm::mat4 viewMatrix(GLFWwindow* window, float width, float height)
{
	double xPos;
	double yPos;
	glfwGetCursorPos(window, &xPos, &yPos); //mouse position
	glfwSetCursorPos(window, width / 2, height / 2); //recentre the mouse

	//compute new orientation
	horizontalAngle += mouseSpeed * float((width / 2) - xPos);
	verticalAngle += mouseSpeed * float((height / 2) - yPos);

	glm::vec3 forwardVec(cos(verticalAngle)*sin(horizontalAngle), //spherical to cartesian conversion
						sin(verticalAngle),
						cos(verticalAngle)*cos(horizontalAngle));
	glm::vec3 sidewardVec = glm::vec3(sin(horizontalAngle - (3.14159f / 2.0f)), 0, cos(horizontalAngle - (3.14159f / 2.0f)));
	glm::vec3 upwardVec = glm::cross(sidewardVec, forwardVec);

	//update our position on key presses
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += forwardVec * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= forwardVec * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position += sidewardVec * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position -= sidewardVec * moveSpeed;

	return glm::lookAt(position, position + forwardVec, upwardVec);
}

glm::mat4 projectionMatrix(GLFWwindow* window, float width, float height)
{
	return glm::perspective(glm::radians(45.0f), width / height, 0.1f, 100.0f);
}