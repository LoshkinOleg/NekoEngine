#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;
in vec2 SideTexCoord;
in vec2 TopTexCoord;
in vec2 BottomTexCoord;

struct Light{
    float intensity;
    vec3 color;
    vec3 direction;
};

struct Material{
    sampler2D diffuse;
};
uniform Light light;
uniform Material material;

uniform sampler2D shadowMap;
uniform vec3 viewPos;
uniform bool enableShadow;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.x < 0.0 || projCoords.x > 1.0 ||projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;
    
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    if(currentDepth > 1.0)
        return 0.0;
    
    vec3 normal = normalize(Normal);
    vec3 lightDir = -light.direction;
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

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
	
    vec3 normal = normalize(Normal);
    //vec3 lightColor = vec3(1.0);
    // ambient
    vec3 ambient = 0.3 * texColor.rgb;

    // diffuse
    vec3 lightDir = -light.direction;
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * light.color * texColor.rgb * light.intensity;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * light.color * light.intensity;

    // calculate shadow
    float shadow;
    if(enableShadow){
        shadow = ShadowCalculation(FragPosLightSpace);   
    }else{
        shadow = 0.0;
    }
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * texColor.rgb;    
    
    FragColor = vec4(lighting, 1.0);
}