#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec3 FaceNormal_0;
layout(location = 3) in vec3 FaceNormal_1;
layout(location = 4) in float onEdge;
layout(location = 5) in vec2 VertexTexCoord;
layout(location = 6) in vec4 VertexTangent;

struct LightInfo {
    vec4 Position;      // Light position in eye coords.
    vec3 Intensity;     // A, D, S intensity
};
uniform LightInfo Light;

out vec3 LightDir;
out vec2 TexCoord;
out vec3 ViewDir;
out float isEdge;

uniform float LineWidthFactor;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;

void main() {
    vec3 normal = normalize(NormalMatrix * VertexNormal);
    vec3 tangent = normalize(NormalMatrix * vec3(VertexTangent));
    vec3 binormal = normalize(cross(normal, tangent)) * VertexTangent.w;
    mat3 toObjectLocal = mat3(
        tangent.x, binormal.x, normal.x,
        tangent.y, binormal.y, normal.y,
        tangent.z, binormal.z, normal.z
    );
    vec3 position;
    vec3 FNormal_0 = NormalMatrix * FaceNormal_0;
    vec3 FNormal_1 = NormalMatrix * FaceNormal_1;
    if (FNormal_0.z * FNormal_1.z < 0 || FaceNormal_0.x == 2.0) {
        position = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0)) + LineWidthFactor * normal;
    }
    else {
        position = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0));
    }
    LightDir = normalize(toObjectLocal * (Light.Position.xyz - position));
    ViewDir = toObjectLocal * normalize(-position);
    TexCoord = VertexTexCoord;
    isEdge = onEdge;
    gl_Position = ProjectionMatrix * vec4(position, 1.0);
}