#version 410 core

out vec4 col;
in vec3 n;
in vec2 uv;
uniform sampler2D t_Albedo;

void main(){
    vec3 nCol = (n + 1.0) * 0.5; // to avoid negative values
    vec4 texCol =  texture(t_Albedo, uv);
    col = vec4(texCol.rgb,1.0);
}