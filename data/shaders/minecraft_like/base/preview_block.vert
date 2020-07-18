#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;

uniform mat4 model;

out vec3 Normal;
out vec2 SideTexCoord;
out vec2 TopTexCoord;
out vec2 BottomTexCoord;

uniform uint sideTexId;
uniform uint topTexId;
uniform uint bottomTexId;

const uint tileSize = 16u;
const uint tileNbr = 16u;

void main()
{
	Normal = aNormal;
	SideTexCoord = (aTexCoords + vec2(sideTexId % tileNbr, float(sideTexId / tileNbr))) / tileNbr;
	if (topTexId != 0u || bottomTexId != 0u)
	{
		TopTexCoord = (aTexCoords + vec2(topTexId % tileNbr, float(topTexId / tileNbr))) / tileNbr;
		BottomTexCoord = (aTexCoords + vec2(bottomTexId % tileNbr, float(bottomTexId / tileNbr))) / tileNbr;
	}
	else
	{
		TopTexCoord = vec2(0.0);
	    BottomTexCoord = vec2(0.0);
	}
	
    vec4 pos = model * vec4(aPos, 1.0);
    gl_Position = pos;
}