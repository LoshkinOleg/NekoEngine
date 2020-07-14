#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec4 FragPosLightSpace;
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
uniform sampler2D shadowMap;
uniform vec3 lightDirection;
uniform vec3 viewPos;
uniform bool enableBias;
uniform bool enableShadow;
uniform bool enableOverSampling;
uniform bool enablePcf;
uniform float bias;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    //outside of light frustum view
    if(enableOverSampling && 
        (projCoords.x < 0.0 || projCoords.x > 1.0 ||projCoords.y < 0.0 || projCoords.y > 1.0))
        return 0.0;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    if(enableOverSampling && currentDepth > 1.0)
        return 0.0;
    if(enablePcf)
    {
        float shadow = 0.0;
        vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
        for(int x = -1; x <= 1; ++x)
        {
            for(int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
                shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
            }    
        }
        shadow /= 9.0;
        return shadow;
    }
    // check whether current frag pos is in shadow
    if(enableBias)
    {
        float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
        return shadow;  
    }
    else
    {
        float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
        return shadow;
    }
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
    
    float shadow = enableShadow ? ShadowCalculation(FragPosLightSpace, normal, lightDir) : 0.0;       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    

	FragColor = vec4(lighting, 1.0);
}