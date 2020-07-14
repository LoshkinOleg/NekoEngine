#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;
in vec2 SideTexCoord;
in vec2 TopTexCoord;
in vec2 BottomTexCoord;

uniform sampler2D diffuse;
uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightDirection;

 float ShadowCalculation(vec4 fragPosLightSpace)
{
    /// perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
   
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    return shadow;
    
}


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
	
    vec3 color = texColor.rgb;
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(1.0);
    // ambient
    vec3 ambient = 0.15 * color;
    // diffuse
    vec3 lightDir = -lightDirection;
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}