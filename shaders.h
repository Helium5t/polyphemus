#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// From https://learnopengl.com/Getting-started/Shaders
// Renamed for personal preference

class Shader{
    public:
	Shader(const std::string& vertexName, const std::string& fragmentName, Shader* fb = NULL);
	~Shader();

	void Bind();

	void SetBool(const std::string& name, bool value) const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;

	void SetVec2(const std::string& name, float x, float y) const;
	void SetVec3(const std::string& name, float x, float y, float z) const;

	void SetVec2(const std::string& name, glm::vec2& vec) const;
	void SetVec3(const std::string& name, glm::vec3& vec) const;
	void SetVec4(const std::string& name, glm::vec4& vec) const;
	void SetMat4(const std::string& name, glm::mat4& mat4) const;

	void SetUseFallback(bool value);
private:
	unsigned int shaderID;

	bool useFallback;
	Shader* fallback;
};