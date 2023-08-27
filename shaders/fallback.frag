#version 410 core

out vec4 col;
in vec3 n;
in vec2 uv;
in vec4 vCol;
uniform vec4 c_Base;
uniform sampler2D t_Albedo;
uniform sampler2D t_Normal;
uniform sampler2D t_Mr;
uniform sampler2D t_Ao;
uniform sampler2D t_Emissive;

uniform int texFlag;

void main(){
    vec3 nCol = (n + 1.0) * 0.5;
    vec4 baseCol = (texFlag & (1 << 0) ) > 0 ? vCol : vec4(0.);
    vec4 normCol = (texFlag & (1 << 1) ) > 0 ? vec4(normalize(nCol),1.0) : vec4(0.);
    col =  normCol + baseCol;
}