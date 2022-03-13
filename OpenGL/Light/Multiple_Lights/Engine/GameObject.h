#pragma once

#include "Core.h"

class GameObject {

protected:

	Transform transform;

public:
	
	GameObject() {}
	
	GameObject(Transform& Source) : transform(Source) {}

	inline Transform GetTransform() const {
		return transform;
	}

	inline Vector3 GetWorldLocation() const {
		return transform.Location;
	}

	inline Vector3 GetWorldRotation() const {
		return transform.Rotation;
	}

	inline Vector3 GetScale() const {
		return transform.Scale;
	}

	void SetTransform(const Transform& Source) {
		transform = Source;
	}

	void SetWorldLocation(const Vector3& location) {
		transform.Location = location;
	}

	void SetWorldRotation(const Vector3& rotation) {
		transform.Rotation = rotation;
	}

	void SetScale(const Vector3& scale) {
		transform.Scale = scale;
	}
};