#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 SideTexCoord;
in vec2 TopTexCoord;
in vec2 BottomTexCoord;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

void main()
{
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