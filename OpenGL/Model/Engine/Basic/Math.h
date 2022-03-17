#pragma once

#include "BasicCore.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef glm::vec3 Vector3;
typedef glm::vec2 Vector2;
typedef glm::mat4 Mat4;

namespace Math {

	inline Mat4 Translate(const Mat4& Source, const Vector3& V3) {
		return glm::translate(Source, V3);
	}

	inline Mat4 Scale(const Mat4& Source, const Vector3& V3) {
		return glm::scale(Source, V3);
	}

	inline float Radians(float Angle) {
		return glm::radians(Angle);
	}

	inline Mat4 Perspective(float FOV, float Aspect, float Near, float Far) {
		return glm::perspective(FOV, Aspect, Near, Far);
	}
}

