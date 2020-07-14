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
uniform float bias;
uniform vec3 lightPos;
uniform vec3 viewPos;


float shadowCalc(float dotLightNormal){
    vec3 pos = FragPosLightSpace.xyz * 0.5 + 0.5;
    if(pos.z > 1.0){
       pos.z = 1.0;
    }
    float depth = texture(shadowMap, pos.xy).r;
    float bias = max(0.05 * (1.0 - dotLightNormal), 0.005);

    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x){
        for(int y = -1; y <= 1; ++y){
            float depth = texture(shadowMap, pos.xy + vec2(x,y) * texelSize).r;
            shadow += (depth + bias) < pos.z ? 0.0 : 1.0;
        }
    }

    return shadow / 9.0;
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
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;   
    float dotLightNormal = dot(lightDir, normal);
    float shadow = shadowCalc(dotLightNormal);
    //float shadow = ShadowCalculation(FragPosLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    

	FragColor = vec4(lighting, 1.0);
}