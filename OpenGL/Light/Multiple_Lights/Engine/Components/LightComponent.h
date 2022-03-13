#pragma once

#include "Core.h"
#include "Engine/GameObject.h"

const static std::string AMBIENT	= ".Ambient";
const static std::string DIFFUSE	= ".Diffuse";
const static std::string SPECULAR	= ".Specular";
const static std::string DIRECTION	= ".Direction";
const static std::string KC			= ".Kc";
const static std::string KL			= ".Kl";
const static std::string KQ			= ".Kq";
const static std::string POSITION	= ".Position";
const static std::string CUTOFF		= ".CutOff";
const static std::string OUTCUTOFF	= ".OutCutOff";

class LightComponent : public GameObject {

public:
	
	LightComponent() {}

	virtual void SetShaderData(Shader& shader, std::string Name) = 0;
};

class DirectLight : public LightComponent {

protected:

	Vector3 Direction;
	Vector3 Ambient;
	Vector3 Diffuse;
	Vector3 Specular;

public:

	DirectLight() : Direction(0.0f), Ambient(0.2f), Diffuse(0.5f), Specular(1.0f) {}

	DirectLight(Vector3 direction, Vector3 ambient, Vector3 diffuse, Vector3 specular) :
		Direction(direction), Ambient(ambient), Diffuse(diffuse), Specular(specular) {}

	inline Vector3 GetDirection() const {
		return Direction;
	}

	inline Vector3 GetAmbient() const {
		return Ambient;
	}

	inline Vector3 GetDiffuse() const {
		return Diffuse;
	}

	inline Vector3 GetSpecular() const {
		return Specular;
	}
	
	void SetDirection(const Vector3& direction) {
		Direction = direction;
	}

	void SetAmbient(const Vector3& ambient) {
		Ambient = ambient;
	}

	void SetDiffuse(const Vector3& diffuse) {
		Diffuse = diffuse;
	}

	void SetSpecular(const Vector3& specular) {
		Specular = specular;
	}

public:

	virtual void SetShaderData(Shader& shader, std::string Name) {
		shader.useUniformVec3((Name + AMBIENT).c_str(), Ambient);
		shader.useUniformVec3((Name + DIFFUSE).c_str(), Diffuse);
		shader.useUniformVec3((Name + SPECULAR).c_str(), Specular);
		shader.useUniformVec3((Name + DIRECTION).c_str(), Direction);
	}
};

class PointLight : public GameObject {

private:

	Vector3 Ambient;
	Vector3 Diffuse;
	Vector3 Specular;

	float Kc;
	float Kl;
	float Kq;

public:

	PointLight() : Ambient(0.2f), Diffuse(0.5f), Specular(1.0f), 
		Kc(1.0f), Kl(0.09f), Kq(0.032f) {}

	PointLight(Vector3 Location, Vector3 ambient, Vector3 diffuse, Vector3 specular, 
		float kc, float kl, float kq) : Ambient(ambient), Diffuse(diffuse), Specular(specular), 
		Kc(kc), Kl(kl), Kq(kq) {
		transform.Location = Location;
	}

	inline Vector3 GetAmbient() const {
		return Ambient;
	}

	inline Vector3 GetDiffuse() const {
		return Diffuse;
	}

	inline Vector3 GetSpecular() const {
		return Specular;
	}

	void SetAmbient(const Vector3& ambient) {
		Ambient = ambient;
	}

	void SetDiffuse(const Vector3& diffuse) {
		Diffuse = diffuse;
	}

	void SetSpecular(const Vector3& specular) {
		Specular = specular;
	}

	inline float GetKc() const {
		return Kc;
	}

	inline float GetKl() const {
		return Kl;
	}

	inline float GetKq() const {
		return Kq;
	}

	void SetKc(float kc) {
		Kc = kc;
	}

	void SetKl(float kl) {
		Kl = kl;
	}

	void SetKq(float kq) {
		Kq = kq;
	}

public:

	virtual void SetShaderData(Shader& shader, std::string Name) {
		shader.useUniformVec3((Name + AMBIENT).c_str(), Ambient);
		shader.useUniformVec3((Name + DIFFUSE).c_str(), Diffuse);
		shader.useUniformVec3((Name + SPECULAR).c_str(), Specular);
		shader.useUniformVec3((Name + POSITION).c_str(), transform.Location);
		shader.useUniformFloat((Name + KC).c_str(), Kc);
		shader.useUniformFloat((Name + KL).c_str(), Kl);
		shader.useUniformFloat((Name + KQ).c_str(), Kq);
	}
};

class SpotLight : public GameObject {

private:

	Vector3 Direction;
	Vector3 Ambient;
	Vector3 Diffuse;
	Vector3 Specular;

	float Kc;
	float Kl;
	float Kq;

	float CutOff;
	float OutCutOff;

public:

	SpotLight() : Direction(0.0f), Ambient(0.2f), Diffuse(0.5f), Specular(1.0f),
		Kc(1.0f), Kl(0.09f), Kq(0.032f) , CutOff(glm::cos(glm::radians(10.f))), 
			OutCutOff(glm::cos(glm::radians(15.f))) {
		std::cout << "SpotLight" << std::endl;
	}

	SpotLight(Vector3 location, Vector3 direction, Vector3 ambient, Vector3 diffuse, Vector3 specular,
		float kc, float kl, float kq, float cutOff, float outCutOff) :
		Direction(direction), Ambient(ambient), Diffuse(diffuse), Specular(specular), Kc(kc),
		Kl(kl), Kq(kq), CutOff(cutOff), OutCutOff(outCutOff) {
		transform.Location = location;
	}

	inline Vector3 GetDirection() const {
		return Direction;
	}

	inline Vector3 GetAmbient() const {
		return Ambient;
	}

	inline Vector3 GetDiffuse() const {
		return Diffuse;
	}

	inline Vector3 GetSpecular() const {
		return Specular;
	}

	void SetDirection(const Vector3& direction) {
		Direction = direction;
	}

	void SetAmbient(const Vector3& ambient) {
		Ambient = ambient;
	}

	void SetDiffuse(const Vector3& diffuse) {
		Diffuse = diffuse;
	}

	void SetSpecular(const Vector3& specular) {
		Specular = specular;
	}

	inline float GetKc() const {
		return Kc;
	}

	inline float GetKl() const {
		return Kl;
	}

	inline float GetKq() const {
		return Kq;
	}

	void SetKc(float kc) {
		Kc = kc;
	}

	void SetKl(float kl) {
		Kl = kl;
	}

	void SetKq(float kq) {
		Kq = kq;
	}

	inline float GetCutOff() const {
		return CutOff;
	}

	inline float GetOutCutOff() const {
		return OutCutOff;
	}

	void SetCutOff(float cutOff) {
		CutOff = cutOff;
	}

	void SetOutCutOff(float outCutOff) {
		OutCutOff = outCutOff;
	}

public:

	virtual void SetShaderData(Shader& shader, std::string Name) {
		shader.useUniformVec3((Name + AMBIENT).c_str(), Ambient);
		shader.useUniformVec3((Name + DIFFUSE).c_str(), Diffuse);
		shader.useUniformVec3((Name + SPECULAR).c_str(), Specular);
		shader.useUniformVec3((Name + DIRECTION).c_str(), Direction);
		shader.useUniformVec3((Name + POSITION).c_str(), transform.Location);
		shader.useUniformFloat((Name + KC).c_str(), Kc);
		shader.useUniformFloat((Name + KL).c_str(), Kl);
		shader.useUniformFloat((Name + KQ).c_str(), Kq);
		shader.useUniformFloat((Name + CUTOFF).c_str(), CutOff);
		shader.useUniformFloat((Name + OUTCUTOFF).c_str(), OutCutOff);
	}
};