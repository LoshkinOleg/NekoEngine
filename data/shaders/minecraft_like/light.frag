#version 300 es
precision highp float;
out vec4 FragColor;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	int shininess;
};

uniform Material objectMaterial;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main()
{
	vec3 ambient = texture(objectMaterial.diffuse, TexCoords).rgb * ambientStrength * lightColor;
	
	vec3 norm = normalize(Normal);
	
	//lightDir is the direction to the light from the fragment
	vec3 lightDir = normalize(lightPos - FragPos); 
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diffuseStrength * diff * lightColor * texture(objectMaterial.diffuse, TexCoords).rgb;

	//viewDir is the direction to the camera from the fragment
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), float(objectMaterial.shininess));
	vec3 specular = specularStrength * spec * lightColor * texture(objectMaterial.specular, TexCoords).rgb;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	FragColor = vec4(result, 1.0);
}