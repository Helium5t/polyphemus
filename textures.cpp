#include <cassert>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include "shaders.h"
#include "textures.h"


Texture::Texture(std::string& path, TexType t, bool sRGB){
    this->type = t;
    buf = stbi_load(path.c_str(), &width, &height, &channelCount, 0);
    assert(buf != nullptr);

    glGenTextures(1, &glID);
    glBindTexture(GL_TEXTURE_2D, glID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    unsigned int f = channelCount == 3 ? 
            (sRGB? GL_SRGB: GL_RGB) : 
            (sRGB? GL_SRGB_ALPHA: GL_RGBA);

    glTexImage2D(GL_TEXTURE_2D, 0, f, width, height, 0, f, GL_UNSIGNED_BYTE, buf);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(buf);
}

void Texture::Bind(const Shader* shader){
    glActiveTexture(GL_TEXTURE0 + (unsigned int) type);
    glBindTexture(GL_TEXTURE_2D, glID);
    shader->SetInt(bindNames[(int)type], (unsigned int) type);
};
