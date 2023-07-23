#version 410 core

out vec4 col;
in vec3 vColor;

void main(){
    col = vec4(vColor, 1.0f);
}