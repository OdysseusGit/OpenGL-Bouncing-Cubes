#pragma once

class Cube
{
private:
	int groundTime = 0;

	double xPos = 0, yPos = 0, zPos = 0;
	double xSpin = 0, ySpin = 0, zSpin = 0;
	double xVelocity = 0, yVelocity = 0, zVelocity = 0;
	double theta = ((double)(rand() % 101) - 50) / 2000, phi = ((double)(rand() % 101) - 50) / 2000;
	double gravity = -0.00981; //-9.81 millimeters per second squared

	float colourGrad = 0.0f;
	float colourIncrement = 0.005f;

	glm::mat4 model;
	glm::mat4 scale(double size);
	glm::mat4 translate(double x, double y, double z);
	glm::mat4 rotate(double xAxis, double yAxis, double zAxis);
public:
	void draw(glm::mat4 projection, glm::mat4 view, GLuint vertexBuffer, GLuint mvpID, GLuint colourID);
};