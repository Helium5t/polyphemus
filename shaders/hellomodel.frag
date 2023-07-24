#version 410 core

out vec4 col;
in vec3 n;

void main(){
    vec3 nCol = (n + 1.0) * 0.5; // to avoid negative values
    col = vec4(nCol, 1.0f);
}