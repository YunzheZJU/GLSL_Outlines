#version 430

struct LightInfo {
    vec4 Position;  // Light position in eye coords.
    vec3 Intensity; // A,D,S intensity
};
uniform LightInfo Light;

struct MaterialInfo {
    vec3 Ka;            // Ambient reflectivity
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ks;            // Specular reflectivity
    float Shininess;    // Specular shininess factor
};
uniform MaterialInfo Material;

uniform float LineWidth;
uniform vec4 LineColor;

in vec3 Normal;
in vec3 Position;
in float Alpha;

layout(location = 0) out vec4 FragColor;

vec3 phongModel(vec3 pos, vec3 norm) {
    vec3 s = normalize(vec3(Light.Position) - pos);
    vec3 v = normalize(vec3(-pos));
    vec3 h = normalize(v + s);
    return Light.Intensity * Material.Ka + Light.Intensity * Material.Kd * max(0.0, dot(s, norm)) +
        Light.Intensity * Material.Ks * pow(max(0.0, dot(h, norm)), Material.Shininess);
}

void main() {
    FragColor = mix(LineColor, vec4(phongModel(Position, Normal), 1.0), Alpha);
}