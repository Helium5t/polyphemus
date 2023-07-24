
#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 VPMatrix;
uniform mat4 MMatrix;

out vec3 n;
void main()
{
    gl_Position = VPMatrix * MMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    n = aNormal;
}
