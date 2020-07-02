#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 5) in vec3 aInstance;

out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;

mat4 translate(vec3 v)
{
    return mat4(
        vec4(1.0,0.0,0.0,0.0),
        vec4(0.0,1.0,0.0,0.0),
        vec4(0.0,0.0,1.0,0.0),
        vec4(v, 1.0)
    );
}

void main()
{
    TexCoords = aTexCoords;
    vec4 pos = projection * view * translate(aInstance) * vec4(aPos, 1.0);
    gl_Position = pos;
}