#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec3 FaceNormal_0;
layout(location = 3) in vec3 FaceNormal_1;
layout(location = 4) in vec3 FaceNormal_2;
layout(location = 5) in vec3 FaceNormal_3;
layout(location = 6) in vec3 FaceNormal_4;
layout(location = 7) in vec3 FaceNormal_5;
layout(location = 8) in vec3 FaceNormal_6;
layout(location = 9) in vec3 FaceNormal_7;
layout(location = 10) in vec3 FaceNormal_8;
layout(location = 11) in float onEdge;

out vec3 VPosition;
flat out vec3 VNormal;
out float isEdge;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main() {
    VNormal = normalize(NormalMatrix * VertexNormal);
    vec3 FNormal_0 = NormalMatrix * FaceNormal_0;
    vec3 FNormal_1 = NormalMatrix * FaceNormal_1;
    vec3 FNormal_2 = NormalMatrix * FaceNormal_2;
    vec3 FNormal_3 = NormalMatrix * FaceNormal_3;
    vec3 FNormal_4 = NormalMatrix * FaceNormal_4;
    vec3 FNormal_5 = NormalMatrix * FaceNormal_5;
    vec3 FNormal_6 = NormalMatrix * FaceNormal_6;
    vec3 FNormal_7 = NormalMatrix * FaceNormal_7;
    vec3 FNormal_8 = NormalMatrix * FaceNormal_8;
    if (FNormal_0.z * FNormal_1.z < 0 ||
        FNormal_1.z * FNormal_2.z < 0 ||
        FNormal_2.z * FNormal_3.z < 0 ||
        FNormal_3.z * FNormal_4.z < 0 ||
        FNormal_4.z * FNormal_5.z < 0 ||
        FNormal_5.z * FNormal_6.z < 0 ||
        FNormal_6.z * FNormal_7.z < 0 ||
        FNormal_7.z * FNormal_8.z < 0 ||
        FNormal_8.z * FNormal_0.z < 0) {
        VPosition = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0)) + 0.02 * VNormal;
    }
    else {
        VPosition = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0));
    }
    isEdge = onEdge;
    gl_Position = ProjectionMatrix * vec4(VPosition, 1.0);
}