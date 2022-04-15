#pragma once

#include "BasicCore.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef glm::vec4 Vector4;
typedef glm::vec3 Vector3;
typedef glm::vec2 Vector2;
typedef glm::mat4 Mat4;
typedef glm::mat3 Mat3;

namespace Math {

	inline Mat4 Translate(const Mat4& Source, const Vector3& V3) {
		return glm::translate(Source, V3);
	}

	inline Mat4 Scale(const Mat4& Source, const Vector3& V3) {
		return glm::scale(Source, V3);
	}

	inline Mat4 Rotate(const Mat4& Source, float Angle, const Vector3& Axis) {
		return glm::rotate(Source, Angle, Axis);
	}

	inline float Radians(float Angle) {
		return glm::radians(Angle);
	}

	inline float Vec3Distance(const Vector3& V1, const Vector3& V2) {
		return glm::length(V1 - V2);
	}

	inline Mat4 Perspective(float FOV, float Aspect, float Near, float Far) {
		return glm::perspective(FOV, Aspect, Near, Far);
	}

	inline Mat4 Ortho(float left, float right, float bottom, float top, float near, float far) {
		return glm::ortho(left, right, bottom, top, near, far);
	}

	inline Mat4 LookAt(Vector3 Start, Vector3 End, Vector3 Up) {
		return glm::lookAt(Start, End, Up);
	}

	inline Vector3 Normalize(const Vector3& Source) {
		return glm::normalize(Source);
	}

	inline void InitRandomSeed(int Seed) {
		std::srand(Seed);
	}

	inline int RandInt() {
		return std::rand();
	}

	inline float Sin(float Value) {
		return std::sin(Value);
	}

	inline float Cos(float Value) {
		return std::cos(Value);
	}
}

