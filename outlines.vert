#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec3 FaceNormal_0;
layout(location = 3) in vec3 FaceNormal_1;
layout(location = 4) in float onEdge;

out vec3 Position;
out vec3 Normal;
out float isEdge;

uniform float LineWidthFactor = 0.01;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main() {
    Normal = normalize(NormalMatrix * VertexNormal);
    vec3 FNormal_0 = NormalMatrix * FaceNormal_0;
    vec3 FNormal_1 = NormalMatrix * FaceNormal_1;
    if (FNormal_0.z * FNormal_1.z < 0) {
        Position = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0)) + LineWidthFactor * Normal;
    }
    else {
        Position = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0));
    }
    isEdge = onEdge;
    gl_Position = ProjectionMatrix * vec4(Position, 1.0);
}