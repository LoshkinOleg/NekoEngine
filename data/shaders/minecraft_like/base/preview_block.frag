#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 SideTexCoord;
in vec2 TopTexCoord;
in vec2 BottomTexCoord;

uniform sampler2D diffuse;

void main()
{
	vec4 texColor;
	if (TopTexCoord != vec2(0.0) || BottomTexCoord != vec2(0.0))
	{
		if (Normal.y > 0.1)
			texColor = texture(diffuse, TopTexCoord);
		else if (Normal.y < -0.1)
			texColor = texture(diffuse, BottomTexCoord);
		else
			texColor = texture(diffuse, SideTexCoord);
	}
	else
		texColor = texture(diffuse, SideTexCoord);

    if (texColor.a < 0.1)
        discard;
		
    float diff = max(dot(Normal, vec3(-0.4, 0.5, 0.2)), 0.0);
    vec3 diffuse = texColor.rgb * diff;
	
    FragColor = vec4(texColor.rgb * 0.5 + diffuse, texColor.a);
}