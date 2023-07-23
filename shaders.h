#ifndef PP_SHADERS
#define PP_SHADERS



constexpr const char* hellotriVertexSource = R"(
#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vColor;
void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vColor = aColor;
}
)";


constexpr const char* hellotriFragmentSource = R"(
#version 410 core

out vec4 col;
in vec3 vColor;

void main(){
    col = vec4(vColor, 1.0f);
}
)";


#endif 