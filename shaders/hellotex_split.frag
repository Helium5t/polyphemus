#version 410 core

in vec3 v_normal;
in vec2 v_uv;
in vec3 v_wPos;
out vec4 col;
// Asset ubo
uniform vec4 c_Base;
uniform sampler2D t_Albedo;
uniform sampler2D t_Normal;
uniform sampler2D t_Mr;
uniform sampler2D t_M;
uniform sampler2D t_R;
uniform sampler2D t_Ao;
uniform sampler2D t_Emissive;
uniform int chan_roughness;
uniform int chan_metallic;  
uniform int chan_ao;
uniform vec4 uv_transform;
uniform bool b_packedMR;

uniform int texFlag;

void main(){
    vec2 uv = (v_uv + uv_transform.xy) * uv_transform.zw;
    vec3 nCol =         (v_normal + 1.0) * 0.5; // to avoid negative values
    vec4 texAlbedo =    (texFlag & (1 << 0) ) > 0 ?  texture(t_Albedo, uv) : vec4(0.);
    vec4 texNormal =    (texFlag & (1 << 1) ) > 0 ?  texture(t_Normal, uv) : vec4(0.);
    texAlbedo =         (texFlag & (1 << 3) ) > 0 ? ((texFlag & (1 << 0) ) > 0 ?  texAlbedo * texture(t_Ao, uv).r: vec4(texture(t_Ao, uv).r) ): texAlbedo;
    vec4 texEmissive =  (texFlag & (1 << 4) ) > 0 ? texture(t_Emissive, uv) : vec4(0.);
    vec4 mSample = texture((b_packedMR ? t_Mr : t_M), uv);
    vec4 rSample = texture((b_packedMR ? t_Mr : t_R), uv);
    int metal_channel = (b_packedMR ? 2 : chan_metallic);
    int roughness_channel = (b_packedMR ? 1 : chan_roughness);
    float texM =        (texFlag & (1 << 5) ) > 0 ?  mSample[metal_channel] :  0.;
    float texR =        (texFlag & (1 << 6) ) > 0 ?  rSample[roughness_channel] : 0.;
    col = texAlbedo + texNormal + texEmissive + vec4(texM) + vec4(texR);
}