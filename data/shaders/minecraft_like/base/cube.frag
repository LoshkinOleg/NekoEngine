#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 SideTexCoord;
in vec2 TopTexCoord;
in vec2 BottomTexCoord;
flat in uint OccludedFace;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

const uint FRONT  = 1u << 0u;
const uint BACK	  = 1u << 1u;
const uint RIGHT  = 1u << 2u;
const uint LEFT	  = 1u << 3u;
const uint TOP	  = 1u << 4u;
const uint BOTTOM = 1u << 5u;

void main()
{
	if (SideTexCoord == vec2(0.0) ||
		(bool(OccludedFace & FRONT)  && Normal.z >  0.1) ||
		(bool(OccludedFace & BACK) 	 && Normal.z < -0.1) ||
		(bool(OccludedFace & RIGHT)  && Normal.x >  0.1) ||
		(bool(OccludedFace & LEFT) 	 && Normal.x < -0.1) ||
		(bool(OccludedFace & TOP) 	 && Normal.y >  0.1) ||
		(bool(OccludedFace & BOTTOM) && Normal.y < -0.1))
        discard;
		
	vec4 texColor;
	if (TopTexCoord != vec2(0.0) || BottomTexCoord != vec2(0.0))
	{
		if (Normal.y > 0.1)
			texColor = texture(material.diffuse, TopTexCoord);
		else if (Normal.y < -0.1)
			texColor = texture(material.diffuse, BottomTexCoord);
		else
			texColor = texture(material.diffuse, SideTexCoord);
	}
	else
		texColor = texture(material.diffuse, SideTexCoord);

    if (texColor.a < 0.1)
        discard;
	
	FragColor = texColor;
}