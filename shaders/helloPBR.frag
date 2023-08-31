#version 410 core

#define EPSILON 1e-5
#define PI 3.14159265358979323846264338327
#define GAMMA_POWER 0.4545454545454545
#define NUM_LIGHTS 1

out vec4 col;
in vec3 v_normal;
in vec2 v_uv;
in vec3 v_wPos;
// Asset ubo
uniform vec4 c_Base;
uniform sampler2D t_Albedo;
uniform sampler2D t_Normal;
uniform sampler2D t_Mr;
uniform sampler2D t_Ao;
uniform sampler2D t_Emissive;
uniform float m_roughness; // constant values just to test things, should actually get values from textures later on.
uniform float m_metallic;  
uniform vec3 m_albedo;

// Light ubo
uniform vec3 l_Pos;
uniform vec3 l_Color;
uniform vec3 l_CamPos;

// UI ubo (for showing specific maps)
uniform int texFlag;

float GGX(vec3 n, vec3 h, float roughness){
    float a = roughness * roughness;
    float a2 = a * a;
    float ndh = max(dot(n, h), 0.);
    float ndh2 = ndh * ndh;

    float den = (ndh2 * (a2 - 1.) + 1.);
    den *= PI * den;
    return a2 / den;
}

float GGX_G(float adotb, float roughness){
    float r = roughness + 1.;
    float k = (r * r) / 8.;
    float den = adotb * (1. - k) + k;

    return adotb/den;
}

// Smith+GGX Approximation of G from https://media.gdcvault.com/gdc2017/Presentations/Hammon_Earl_PBR_Diffuse_Lighting.pdf
float GGXSmithApproximated(vec3 n, vec3 v, vec3 l, float roughness){
    float ndv = max(dot(n,v), 0.);
    float ndl = max(dot(n,l), 0.);

    float ggx1 = GGX_G(ndl, roughness);
    float ggx2 = GGX_G(ndv, roughness);

    return ggx1 * ggx2;
}

vec3 SchlickFresnel(vec3 a, vec3 b, vec3 f0)
{
    float cosTheta = max(dot(a,b), 1.0);
    return f0 + (1.0 - f0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


vec4 BRDF(vec3 f0, vec3 n, vec3 v, float roughness, float metallic, vec3 albedo){
    vec3 lOut = vec3(0.);

    for(int i=0; i < NUM_LIGHTS; i++){
        // Assume point light model
        vec3 l = normalize(l_Pos - v_wPos);
        vec3 h = normalize( v + l);
        float dist = length(l_Pos - v_wPos);
        float attenuation = 1. / dist*dist;
        vec3 lRadiance = l_Color * attenuation;

        // Cook-Torrance model with GGX NDF, Smith Masking, Schlick Approximated Fresnel
        float D = GGX(n, h, roughness);
        float G = GGXSmithApproximated(n, v, l, roughness);
        vec3 F = SchlickFresnel(h, v, f0);

        float ndl = max(dot(n,l), 0.);

        float denominator = 4. * max(dot(n,v), 0.) * ndl + EPSILON; // epsilon avoid dividing by 0
        vec3 spec = (F * G * D) / denominator;

        vec3 kSpec = F;
        vec3 kDiff = vec3(1.) - kSpec; // Ensures energy conservation
        kDiff *= 1. - metallic; // Metals do not scatter light via internal reflection

        lOut = (kDiff * albedo / PI + spec) * lRadiance * ndl;
    }

    vec3 color = 50.0 * EPSILON * albedo + lOut; // adding albedo just to avoid having full black models

    return vec4(color, 1.);
}

void main(){
    // vec3 nCol = (v_normal + 1.0) * 0.5; // to avoid negative values
    // vec4 texAlbedo =  (texFlag & (1 << 0) ) > 0 ?  texture(t_Albedo, uv) * c_Base : vec4(0.);
    // vec4 texNormal =  (texFlag & (1 << 1) ) > 0 ?  texture(t_Normal, uv) : vec4(0.);
    // vec4 texMr =      (texFlag & (1 << 2) ) > 0 ?  texture(t_Mr, uv) : vec4(0.);
    // texAlbedo =      (texFlag & (1 << 3) ) > 0 ? ((texFlag & (1 << 0) ) > 0 ?  texAlbedo * texture(t_Ao, uv).r: vec4(texture(t_Ao, uv).r) ): texAlbedo;
    // vec4 texEmissive = (texFlag & (1 << 4) ) > 0 ? texture(t_Emissive, uv) : vec4(0.);

    vec3 f0 = mix(vec3(0.04),m_albedo, m_metallic);
    vec3 v = normalize(l_CamPos - v_wPos);
    vec4 brdfCol = BRDF(f0, v_normal, v, m_roughness, m_metallic, m_albedo);

    // HDR + gamma
    brdfCol.xyz /= (brdfCol.xyz + vec3(1.));
    brdfCol.xyz = pow(brdfCol.xyz, vec3(GAMMA_POWER));
    // col = texAlbedo + texNormal + texMr + texEmissive;

    col = brdfCol;
}