#ifndef VBOMESH_H
#define VBOMESH_H

#include "global.h"
#include "drawable.h"

#include <string>
#include <vector>

using namespace std;

class VBOMesh : public Drawable {
private:
    GLuint faces;
    GLuint vaoHandle;

    bool reCenterMesh, loadTex, genTang;

    void trimString(string &str);

    void storeVBO(const vector<vec3> &points,
                  const vector<vec3> &normals,
                  const vector<vec2> &texCoords,
                  const vector<vec4> &tangents,
                  const vector<GLuint> &elements,
                  const vector<vector<vec3>> &faceNormals,
                  const vector<float> &onEdge);

    void generateAveragedNormals(
            const vector<vec3> &points,
            vector<vec3> &normals,
            const vector<GLuint> &faces);

    void generateNormals(
            const vector<vec3> &points,
            const vector<GLuint> &faces,
            vector<vector<vec3>> &faceNormals);

    void generateTangents(
            const vector<vec3> &points,
            const vector<vec3> &normals,
            const vector<GLuint> &faces,
            const vector<vec2> &texCoords,
            vector<vec4> &tangents);

    void center(vector<vec3> &);

    void addQuads(
            vector<vec3> &points,
            vector<vec3> &normals,
            vector<GLuint> &faces,
            vector<vector<vec3>> &faceNormals,
            vector<float> &onEdge);

    void addSingleQuad(
            GLuint a1,
            GLuint b1,
            vector<vec3> &points,
            vector<vec3> &normals,
            vector<GLuint> &faces,
            vector<vector<vec3>> &faceNormals,
            vector<float> &onEdge
    );

public:
    VBOMesh(const char *fileName, bool reCenterMesh = false, bool loadTc = false, bool genTangents = false);

    void render() const;

    void loadOBJ(const char *fileName);
};

#endif // VBOMESH_H
