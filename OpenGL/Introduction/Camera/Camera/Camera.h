#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

typedef glm::vec3 v3;

// enum Camera Movement Direction
enum Camera_Movement_Dir {
	FORWORD,
	BACKWORD,
	LEFT,
	RIGHT
};

// Default Camera Values
const float YAW			= -90.0f;
const float PITCH		= 0.0f;
const float SPEED		= 2.5f;
const float SENSITIVITY = 0.04f;
const float ZOOM		= 45.0f;

class Camera {

	static constexpr float maxPitch = 89.f;
	static constexpr float minPitch = -89.f;
	static constexpr float minFOV = 1.f;
	static constexpr float maxFOV = 45.f;

private:
	// Camera Attributes
	v3 position;
	v3 frontVector;
	v3 upVector;
	v3 rightVector;
	v3 worldUpVector;

	// Euler Angles
	float Yaw, Pitch;

	// Camera options
	float movementSpeed;
	float mouseSensitivity;
	float Zoom;

public:
	// Constructor
	Camera(v3 pos = v3(0.0f, 0.0f, 0.0f), v3 up = v3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) :
		position(pos), worldUpVector(up), frontVector(v3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), Zoom(Zoom),
		Yaw(yaw), Pitch(pitch) {
		updateCameraVectors();

		std::cout << (position.x) << ' ' << (position.y) << ' ' << (position.z) << std::endl;
		std::cout << (frontVector.x) << ' ' << (frontVector.y) << ' ' << (frontVector.z) << std::endl;
	}

	// Constructor

	// Get lookAt Mat
	inline glm::mat4 getViewMat() const {
		return glm::lookAt(position, position + frontVector, upVector);
	}

	// Get Camera FOV
	inline float getFOV() const {
		return Zoom;
	}

	// Process Keyboard Input
	void processKeyboardInput(Camera_Movement_Dir movementDir, float deltaTime) {
		float val = movementSpeed * deltaTime;
		switch (movementDir)
		{
		case FORWORD:
			position += frontVector * val;
			break;
		case BACKWORD:
			position -= frontVector * val;
			break;
		case LEFT:
			position -= rightVector * val;
			break;
		case RIGHT:
			position += rightVector * val;
			break;
		default:
			break;
		} 
	}

	// Process Mouse Move
	void processMouseMovement(float xOffset, float yOffset, bool contrainPitch = true) {
		xOffset *= mouseSensitivity;
		yOffset *= mouseSensitivity;
		Yaw += xOffset;
		Pitch += yOffset;
		if (contrainPitch) {
			if (Pitch > maxPitch)
				Pitch = maxPitch;
			if (Pitch < minPitch)
				Pitch = minPitch;
		}
		updateCameraVectors();
	}

	// Process Mouse Scroll
	void processMouseScroll(float yOffset) {
		Zoom -= yOffset;
		if (Zoom < minFOV)
			Zoom = minFOV;
		if (Zoom > maxFOV)
			Zoom = maxFOV;
	}

	// Process Key E -> up camera position
	void processKeyE(float deltaTime) {
		position.y += deltaTime * movementSpeed;
	}

private:
	// get camera front vector using euler angles
	void updateCameraVectors() {
		v3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		frontVector = glm::normalize(front);
		rightVector = glm::normalize(glm::cross(frontVector, worldUpVector));
		upVector = glm::normalize(glm::cross(rightVector, frontVector));
	}
};