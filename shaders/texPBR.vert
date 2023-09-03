
#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 VPMatrix;
uniform mat4 MMatrix;

out vec3 v_normal;
out vec2 v_uv;
out vec3 v_wPos;
void main()
{
    // v_wPos = MMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vec4 wPos =  MMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    v_wPos = wPos.xyz;
    gl_Position = VPMatrix * wPos;
    v_uv = aTexCoord;
    v_normal = mat3(transpose(inverse(MMatrix))) * aNormal;
}
