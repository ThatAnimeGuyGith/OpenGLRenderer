#version 330 core
out vec4 FragColor;
in vec3 normal;
uniform vec3 baseColor;
uniform vec3 lightDirection;
uniform float ambientStrength;
uniform float diffuseStrength;
void main()
{
    float directionLength = length(lightDirection);
    vec3 safeLightDirection = directionLength > 0.0001 ? lightDirection / directionLength : vec3(0.0, 1.0, 0.0);
    float diffuse = max(dot(normalize(normal), safeLightDirection), 0.0);
    float lighting = clamp(ambientStrength, 0.0, 1.0) + clamp(diffuseStrength, 0.0, 1.0) * diffuse;
    FragColor = vec4(baseColor * lighting, 1.0);
}
