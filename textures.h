#pragma once
#include <string>

class Shader;

#define PP_TEX_ALBEDO "t_Albedo"
#define PP_TEX_NORMAL "t_Normal"
#define PP_TEX_MR     "t_Mr"
#define PP_TEX_AO     "t_Ao"

enum class TexType{
    Albedo = 0,
    Normal = 1,
    MR = 2, // Metallic / Roughness
    AO = 3
};

class Texture{

    public:
        Texture(std::string& path, TexType t);
        void Bind(const Shader* shader);
    private:
        TexType type;
        unsigned int glID;
        int height;
        int width;
        int channelCount;
        unsigned char *buf;

        std::string bindNames[4]{
            PP_TEX_ALBEDO,
            PP_TEX_NORMAL,
            PP_TEX_MR,
            PP_TEX_AO
        };
};
