#version 330 core
out vec4 FragColor;
in vec3 normal;

void main()
{
    const vec3 baseColor = vec3(0.82, 0.62, 0.38);
    vec3 lightDirection = normalize(vec3(0.4, 0.8, 0.5));
    float diffuse = max(dot(normalize(normal), lightDirection), 0.0);
    float lighting = 0.24 + 0.76 * diffuse;
    FragColor = vec4(baseColor * lighting, 1.0);
}