#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 5) in uint blockId;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;
uniform vec3 chunkPos;

const uint tileNbr = 16u;
const uint chunkSize = 16u;

mat4 translate(vec3 v)
{
    return mat4(
        vec4(1.0,0.0,0.0,0.0),
        vec4(0.0,1.0,0.0,0.0),
        vec4(0.0,0.0,1.0,0.0),
        vec4(v, 1.0)
    );
}

vec3 idToPos(uint id)
{
	return vec3(
		id % tileNbr,
		(id / tileNbr) % tileNbr,
		id / (tileNbr * tileNbr)
	) + chunkPos * chunkSize;
}

void main()
{
    gl_Position = lightSpaceMatrix * translate(idToPos(blockId)) * vec4(aPos, 1.0);
}  