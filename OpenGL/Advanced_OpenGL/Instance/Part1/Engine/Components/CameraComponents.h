#pragma once

#include "../Basic/Core.h"

// enum Camera Movement Direction
enum Camera_Movement_Dir {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default Camera Values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 1.5f;
const float SENSITIVITY = 0.04f;
const float ZOOM = 45.0f;

class Camera {

	static constexpr float maxPitch = 89.f;
	static constexpr float minPitch = -89.f;
	static constexpr float minFOV = 1.f;
	static constexpr float maxFOV = 45.f;

private:
	// Camera Attributes
	Vector3 position;
	Vector3 frontVector;
	Vector3 upVector;
	Vector3 rightVector;
	Vector3 worldUpVector;

	// Euler Angles
	float Yaw, Pitch;

	// Camera options
	float movementSpeed;
	float mouseSensitivity;
	float Zoom;

public:
	// Constructor
	Camera(Vector3 pos = Vector3(0.0f, 0.0f, 0.0f), Vector3 up = Vector3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) :
		position(pos), worldUpVector(up), frontVector(Vector3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), Zoom(ZOOM),
		Yaw(yaw), Pitch(pitch) {
		updateCameraVectors();
	}

	// Get lookAt Mat
	inline Mat4 getViewMat() const {
		return glm::lookAt(position, position + frontVector, upVector);
	}

	// Get Camera FOV
	inline float getFOV() const {
		return Zoom;
	}

	// Get Camera Position
	inline Vector3 getCameraPos() const {
		return position;
	}

	inline Vector3 getFrontVec3() const {
		return frontVector;
	}

	// Process Keyboard Input
	void processKeyboardInput(Camera_Movement_Dir movementDir, float deltaTime) {
		float val = movementSpeed * deltaTime;
		switch (movementDir)
		{
		case FORWARD:
			position += frontVector * val;
			break;
		case BACKWARD:
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
		Vector3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		frontVector = glm::normalize(front);
		rightVector = glm::normalize(glm::cross(frontVector, worldUpVector));
		upVector = glm::normalize(glm::cross(rightVector, frontVector));
	}
};