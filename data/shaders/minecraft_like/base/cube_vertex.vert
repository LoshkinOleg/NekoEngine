#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;
layout(location = 5) in uint blockId;
layout(location = 6) in uint blockHash;

out vec3 Normal;
out vec2 SideTexCoord;
out vec2 TopTexCoord;
out vec2 BottomTexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 chunkPos;

const uint chunkSize = 16u;
const uint tileSize = 16u;
const uint tileNbr = 16u;

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
	Normal = aNormal;
	
	uint sideTexId = blockHash >> 16 & 0xFFu;
	uint topTexId = blockHash >> 8 & 0xFFu;
	uint bottomTexId = blockHash & 0xFFu;
	if (sideTexId != 0u)
		SideTexCoord = (aTexCoords + vec2(sideTexId % tileNbr, sideTexId / tileNbr)) / tileNbr;
	else
		SideTexCoord = vec2(0.0);
		
	if (topTexId != 0u || bottomTexId != 0u)
	{
		TopTexCoord = (aTexCoords + vec2(topTexId % tileNbr, topTexId / tileNbr)) / tileNbr;
		BottomTexCoord = (aTexCoords + vec2(bottomTexId % tileNbr, bottomTexId / tileNbr)) / tileNbr;
	}
	else
	{
		TopTexCoord = vec2(0.0);
	    BottomTexCoord = vec2(0.0);
	}
	
	vec3 blockPos = idToPos(blockId);
    vec4 pos = projection * view * translate(blockPos) * vec4(aPos, 1.0);
    gl_Position = pos;
}