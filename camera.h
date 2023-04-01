#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
enum MovementDirection
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};
class Camera
{
public:
	float yaw, pitch, fov, sensitivity, moveSpeed;
	glm::vec3 cameraPos, cameraFront, cameraUp;
	bool firstMouse = true;


	Camera(glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f),
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f),
		float fov = 75.0f,
		float pitch = 0.0f,
		float yaw = -90.0f, // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
		float sensitivity = 0.1,
		float moveSpeed = 2.5)
	{
		this->cameraPos = cameraPos;
		this->cameraFront = cameraFront;
		this->cameraUp = cameraUp;
		this->pitch = pitch;
		this->yaw = yaw;
		this->fov = fov;
		this->sensitivity = sensitivity;
		this->moveSpeed = moveSpeed;
	}

	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); // cameraPos + cameraFront ensure the camera keeps looking at the same spot while moving
	}

	void keyboardMovement(MovementDirection direction, float deltaTime)
	{
		float velocity = moveSpeed * deltaTime;
		if (direction == FORWARD)
			cameraPos += velocity * cameraFront;
		if (direction == BACKWARD)
			cameraPos -= velocity * cameraFront;
		if (direction == LEFT)
			cameraPos += velocity * glm::cross(cameraUp, cameraFront);
		if (direction == RIGHT)
			cameraPos -= velocity * glm::cross(cameraUp, cameraFront);
		if (direction == UP)
			cameraPos += velocity * cameraUp;
		if (direction == DOWN)
			cameraPos -= velocity * cameraUp;

	}

	void mouseMovement(float *lastX, float *lastY, double xposIn, double yposIn)
	{

		float xpos = (float)xposIn;
		float ypos = (float)yposIn;

		if (firstMouse)
		{
			*lastX = xpos;
			*lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - *lastX;
		float yoffset = *lastY - ypos; // swap this around for inverted (flight stick) style controls
		*lastX = xpos;
		*lastY = ypos;

		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);
	}
};