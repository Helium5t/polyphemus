#version 410 core

out vec4 col;
in vec3 n;
in vec2 uv;
uniform vec4 c_Base;
uniform sampler2D t_Albedo;
uniform sampler2D t_Normal;
uniform sampler2D t_Mr;
uniform sampler2D t_M;
uniform sampler2D t_R;
uniform sampler2D t_Ao;
uniform sampler2D t_Emissive;
uniform bool b_packedMR;

uniform int texFlag;

void main(){
    vec3 nCol = (n + 1.0) * 0.5; // to avoid negative values
    vec4 texAlbedo =  (texFlag & (1 << 0) ) > 0 ?  texture(t_Albedo, uv) : vec4(0.);
    vec4 texNormal =  (texFlag & (1 << 1) ) > 0 ?  texture(t_Normal, uv) : vec4(0.);
    // vec4 texMr =      (texFlag & (1 << 2) ) > 0 ?  texture(t_Mr, uv) : vec4(0.);
    texAlbedo =      (texFlag & (1 << 3) ) > 0 ? ((texFlag & (1 << 0) ) > 0 ?  texAlbedo * texture(t_Ao, uv).r: vec4(texture(t_Ao, uv).r) ): texAlbedo;
    vec4 texEmissive = (texFlag & (1 << 4) ) > 0 ? texture(t_Emissive, uv) : vec4(0.);
    vec4 texR = vec4(0.), texM = vec4(0.);
    if (b_packedMR){
        texM =      (texFlag & (1 << 5) ) > 0 ?  vec4(texture(t_Mr, uv).b) : vec4(0.);
        texR =      (texFlag & (1 << 6) ) > 0 ?  vec4(texture(t_Mr, uv).g) : vec4(0.);
    }else{
        texM =      (texFlag & (1 << 5) ) > 0 ?  texture(t_M, uv) : vec4(0.);
        texR =      (texFlag & (1 << 6) ) > 0 ?  texture(t_R, uv) : vec4(0.);
    }
    col = texAlbedo + texNormal + texM + texR + texEmissive;
}