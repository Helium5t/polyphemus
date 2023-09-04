#pragma once
#include <string>

class Shader;

#define PP_TEX_ALBEDO   "t_Albedo"
#define PP_TEX_NORMAL   "t_Normal"
#define PP_TEX_MR       "t_Mr"
#define PP_TEX_M        "t_M"
#define PP_TEX_R        "t_R"
#define PP_TEX_AO       "t_Ao"
#define PP_TEX_EMISSIVE "t_Emissive"

enum class TexType{
    Albedo = 0,
    Normal = 1,
    MR = 2, // Metallic / Roughness
    AO = 3,
    Emissive = 4,
    Metallic = 5,
    Roughness = 6,
};

static std::string tex_name(TexType tt){
    switch (tt){
        case TexType::Albedo:
            return "Albedo";
        case TexType::Normal:
            return "Normal";
        case TexType::MR:
            return "MR";
        case TexType::AO:
            return "AO";
        case TexType::Emissive:
            return "Emissive";
        case TexType::Metallic:
            return "Metallic";
        case TexType::Roughness:
            return "Roughness";
    }
    return "Unknown";
}

class Texture{

    public:
        Texture(std::string& path, TexType t, bool sRGB = false);
        void Bind(const Shader* shader);
    private:
        TexType type;
        unsigned int glID;
        int height;
        int width;
        int channelCount;
        unsigned char *buf;

        std::string bindNames[7]{
            PP_TEX_ALBEDO,
            PP_TEX_NORMAL,
            PP_TEX_MR,
            PP_TEX_AO,
            PP_TEX_EMISSIVE,
            PP_TEX_M,
            PP_TEX_R,
        };
};
