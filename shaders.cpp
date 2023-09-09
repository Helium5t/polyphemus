#include "shaders.h"
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <iostream>


Shader::Shader(const std::string& vertexFileName, const std::string& fragmentFileName, Shader* fb){
    // File Loading
	std::string vCode;
	std::string fCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	std::string vertexPath = "shaders/" + vertexFileName;
	std::string fragmentPath = "shaders/" + fragmentFileName;

    try{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);


		std::stringstream vShaderStream;
		std::stringstream fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vCode = vShaderStream.str();
        fCode = fShaderStream.str();
    }catch(std::ifstream::failure & e){
        std::string errMsg = "[SETUP][ERR][SHADER][FILE][STREAM] error reading shader files:";
        errMsg += e.what();
        throw std::runtime_error(errMsg);
    }


    unsigned int vID, fID;
    int ok;
    char compileLog[512];

    // Stage Compilation
    const char* vShaderCode = vCode.c_str();
    const char* fShaderCode = fCode.c_str();

    vID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vID, 1, &vShaderCode, NULL);
    glCompileShader(vID);

    glGetShaderiv(vID, GL_COMPILE_STATUS, &ok);
    if(!ok){
        glGetShaderInfoLog(vID, 512, NULL, compileLog);
        throw std::runtime_error("[SETUP][ERR][SHADER][VERTEX][COMPILATION] Failed:" + std::string(compileLog) + "(" + vertexFileName + ")");
    }

    fID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fID, 1, &fShaderCode, NULL);
    glCompileShader(fID);

    glGetShaderiv(fID, GL_COMPILE_STATUS, &ok);
    if(!ok){
        glGetShaderInfoLog(fID, 512, NULL, compileLog);
        throw std::runtime_error("[SETUP][ERR][SHADER][FRAGMENT][COMPILATION] Failed:" + std::string(compileLog) + "(" + fragmentFileName + ")");
    }
    // Linking

    shaderID = glCreateProgram();
    glAttachShader(shaderID, vID);
    glAttachShader(shaderID, fID);
    glLinkProgram(shaderID);


    glGetProgramiv(shaderID, GL_LINK_STATUS, &ok);
    if(!ok){
        glGetProgramInfoLog(shaderID, 512, NULL, compileLog);
        throw std::runtime_error("[SETUP][ERR][SHADER][LINKING][COMPILATION] Failed: " + std::string(compileLog));
    }

    if(fb != NULL){
        fallback = fb;
    }


    // Cleanup
    glDeleteShader(vID);
    glDeleteShader(fID);
}


Shader::~Shader(){
    glDeleteProgram(shaderID);
}

void Shader::Bind(){
    if (useFallback){
        fallback->Bind();
        return;
    }
    glUseProgram(shaderID);
}

void Shader::SetBool(const std::string& name, bool value) const
{
    if(useFallback){
        fallback->SetBool(name, value);
        return;
    }
	glUniform1i(glGetUniformLocation(shaderID, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const
{
    if(useFallback){
        fallback->SetInt(name, value);
        return;
    }
	glUniform1i(glGetUniformLocation(shaderID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
    if(useFallback){
        fallback->SetFloat(name, value);
        return;
    }
	glUniform1f(glGetUniformLocation(shaderID, name.c_str()), value);
}


void Shader::SetVec2(const std::string& name, float x, float y) const
{
    if(useFallback){
        fallback->SetVec2(name, x, y);
        return;
    }
	glUniform2f(glGetUniformLocation(shaderID, name.c_str()), x, y);
}

void Shader::SetVec2(const std::string& name, glm::vec2& vec) const
{
    if(useFallback){
        fallback->SetVec2(name, vec);
        return;
    }
	glUniform2f(glGetUniformLocation(shaderID, name.c_str()), vec.x, vec.y);
}

void Shader::SetVec3(const std::string& name, float x, float y, float z) const
{
    if(useFallback){
        fallback->SetVec3(name, x, y ,z);
        return;
    }
	glUniform3f(glGetUniformLocation(shaderID, name.c_str()), x, y, z);
}

void Shader::SetVec3(const std::string& name, glm::vec3& vec) const
{
    if(useFallback){
        fallback->SetVec3(name, vec);
        return;
    }
	glUniform3f(glGetUniformLocation(shaderID, name.c_str()), vec.x, vec.y, vec.z);
}

void Shader::SetVec4(const std::string& name, glm::vec4& vec) const
{
    if(useFallback){
        fallback->SetVec4(name, vec);
        return;
    }
	glUniform4f(glGetUniformLocation(shaderID, name.c_str()), vec.x, vec.y, vec.z, vec.w);
}

void Shader::SetMat4(const std::string& name, glm::mat4& mat4) const
{
    if(useFallback){
        fallback->SetMat4(name, mat4);
        return;
    }
	glUniformMatrix4fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat4));
}

void Shader::SetUseFallback(bool value){
    useFallback = value;
}