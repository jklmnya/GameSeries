#pragma once

#include "Core.h"

struct Transform {

	Vector3 Location;
	Vector3 Rotation;
	Vector3 Scale;

	Transform() : Location(0.0f), Rotation(0.0f), Scale(1.0f) {}

	Transform(Vector3 location, Vector3 rotation, Vector3 scale) : Location(location), Rotation(rotation), Scale(scale) {}

	Transform(const Transform& Source) : Location(Source.Location), Rotation(Source.Rotation), 
		Scale(Source.Scale) {}

	Transform& operator=(const Transform& Source) noexcept {
		if (this != &Source) {
			Location = Source.Location;
			Rotation = Source.Rotation;
			Scale = Source.Scale;
		}
		return *this;
	}
};