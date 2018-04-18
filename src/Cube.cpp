#include <iostream>

#include <GL/glew.h>

#include "glm/glm.hpp"

#include "Cube.h"

void Cube::draw(glm::mat4 projection, glm::mat4 view, GLuint vertexBuffer, GLuint mvpID, GLuint colourID)
{
	////////////////////////////////////////COLLISION DETECTION////////////////////////////////////////
	//collision detection is determined by the corner vertices
	glm::vec4 corner[8];
	corner[0] = model * glm::vec4(-1, -1, -1, 1);
	corner[1] = model * glm::vec4(-1, -1, 1, 1);
	corner[2] = model * glm::vec4(1, -1, -1, 1);
	corner[3] = model * glm::vec4(1, -1, 1, 1);
	corner[4] = model * glm::vec4(-1, 1, -1, 1);
	corner[5] = model * glm::vec4(-1, 1, 1, 1);
	corner[6] = model * glm::vec4(1, 1, -1, 1);
	corner[7] = model * glm::vec4(1, 1, 1, 1);

	glm::vec4 center;
	for (int i = 0; i < 4; i++)
		center[i] = (corner[0][i] + corner[7][i]) / 2;

	double maxDepth = -30;
	int collisionCount = 0;
	for (int i = 0; i < 8; i++)
	{
		if (corner[i][1] < -30)
		{
			//compute the lowest point
			if (corner[i][1] < maxDepth)
				maxDepth = corner[i][1];

			//increment the number of grounded corners
			collisionCount++;
		}
	}

	if (collisionCount > 0)
	{
		//align the model's height with the ground
		yPos += -30 - maxDepth;

		if (groundTime > 0)
		{
			//the cube is resting on the ground
			xVelocity = 0;
			yVelocity = 0;
			zVelocity = 0;

			theta = 0; //gracefull falling could be implemented here
			phi = 0;

			gravity = 0;
		}
		else
		{
			//calculate the impact vectors
			glm::vec3 impact;
			for (int i = 0; i < 8; i++)
			{
				if (corner[i][1] < -30)
				{
					//compute the normal vector to the corner
					for (int j = 0; j < 3; j++)
						impact[j] += center[j] - corner[i][j];
				}
			}

			//normalise the impact vector
			glm::vec3 normal;
			for (int i = 0; i < 3; i++)
				normal[i] = impact[i] / (abs(impact[i]) + abs(impact[(i + 1) % 3]) + abs(impact[(i + 2) % 3]));

			//adjust the velocities with respect to the normal vectors
			xVelocity = normal[0];
			yVelocity = normal[1] * -0.8 * yVelocity; //normal contribution * ellastic contribution
			zVelocity = normal[2];

			//adjust the spin with respect to the normal vectors
			theta = theta + -0.5 * normal[2]; //arbitrary dampening coefficient
			phi = phi + 0.5 * normal[0];
		}
		groundTime++;
	}
	else
	{
		groundTime = 0;
	}

	//calculate the new trajectory
	xPos = xPos + xVelocity;
	yPos = yPos + yVelocity + 0.5 * gravity;
	zPos = zPos + zVelocity;

	yVelocity = yVelocity + gravity;

	xSpin += theta;
	zSpin += phi;
	///////////////////////////////////////////////////////////////////////////////////////////////////

	//////////COLOUR ANIMATION//////////
	if (colourGrad > 1.0f)
		colourIncrement = -0.005f;
	if (colourGrad < 0.0f)
		colourIncrement = 0.005f;

	colourGrad += colourIncrement;
	////////////////////////////////////

	//decompose the cube movement into translation, rotation and scaling matrices
	model = translate(xPos, yPos, zPos) * rotate(xSpin, ySpin, zSpin) * scale(1);

	//scale the model to its 2D projection
	glm::mat4 mvp = projection * view * model; //MVP = P * V * M

	glEnableVertexAttribArray(0); //position attribute
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//send our transformations to the buffer
	glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);
	glUniform4f(colourID, colourGrad, 0.2f, 0.8f, 0.8f);

	//draw the cube
	glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

	glDisableVertexAttribArray(0);
}

glm::mat4 Cube::scale(double size)
{
	return { { size, 0,	  0,	0 },
			 { 0,	size, 0,	0 },
			 { 0,	0,	  size, 0 },
			 { 0,	0,	  0,	1 } };
}

glm::mat4 Cube::translate(double x, double y, double z)
{
	glm::mat4 transpose = { { 1, 0, 0, x },
							{ 0, 1,	0, y },
							{ 0, 0, 1, z },
							{ 0, 0, 0, 1 } };

	return glm::transpose(transpose); //OpenGL expects column-major ordering
}

glm::mat4 Cube::rotate(double xAxis, double yAxis, double zAxis) //theta, phi variables are less practical?
{
	glm::mat4 xSpin = { { 1,		   0,			0,			0 },
						{ 0,		   cos(xAxis),	sin(xAxis), 0 },
						{ 0,		   -sin(xAxis),	cos(xAxis), 0 },
						{ 0,		   0,			0,			1 } };

	glm::mat4 ySpin = { { cos(yAxis),  0,			sin(yAxis), 0 },
						{ 0,		   1,			0,			0 },
						{ -sin(yAxis), 0,			cos(yAxis), 0 },
						{ 0,		   0,			0,			1 } };

	glm::mat4 zSpin = { { cos(zAxis),  sin(zAxis),	0,			0 },
						{ -sin(zAxis), cos(zAxis),	0,			0 },
						{ 0,		   0,			1,			0 },
						{ 0,		   0,			0,			1 } };

	return glm::transpose(xSpin * ySpin * zSpin);
}