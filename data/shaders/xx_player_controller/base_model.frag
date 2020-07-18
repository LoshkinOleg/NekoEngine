#version 330 core
out vec4 FragColor;

uniform sampler2D tex;

in vec2 TexCoords;
in vec3 Normal;

void main()
{
    vec3 ambient = texture(tex, TexCoords).rgb * 0.2;

    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, vec3(-0.4, 0.5, 0.2)), 0.0);
    vec3 diffuse = texture(tex, TexCoords).rgb * diff;

    FragColor = vec4(ambient + diffuse, 1.0);
}