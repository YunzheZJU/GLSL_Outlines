#ifndef VBOMESH_H
#define VBOMESH_H

#include "global.h"
#include "drawable.h"
//#include "threads.h"

#include <thread>
#include <mutex>
#include <string>
#include <vector>

using namespace std;

class VBOMesh : public Drawable {
private:
    GLuint faceNum;
    GLuint vaoHandle;

    bool reCenterMesh, loadTex, genTang;

    void trimString(string &str);

    void storeVBO(const vector<vec3> &points,
                  const vector<vec3> &normals,
                  const vector<vec2> &texCoords,
                  const vector<vec4> &tangents,
                  const vector<GLuint> &elements,
                  const vector<vector<vec3>> &normalsBeside,
                  const vector<float> &onEdge);

    void generateAveragedNormals(
            const vector<vec3> &points,
            vector<vec3> &normals,
            const vector<GLuint> &faces);

    void generateNormals(
            const vector<vec3> &points,
            const vector<GLuint> &faces,
            vector<vec3> &faceNormals);

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
            vector<vector<vec3>> &normalsBeside,
            vector<float> &onEdge,
            vector<vec3> &faceNormals);

public:
    explicit VBOMesh(const char *fileName, bool reCenterMesh = false, bool loadTc = false, bool genTangents = false);

    void render() const override;

    void loadOBJ(const char *fileName);

    void addSingleQuad(
            GLuint a1,
            GLuint b1,
            vector<vec3> &points,
            vector<vec3> &normals,
            vector<GLuint> &faces,
            vector<vector<vec3>> &faceNormals,
            vector<float> &onEdge,
            vector<vec3> &faceNormal,
            int i,
            int j);
};


void create(
        int start,
        int end);

void addSingleQuad(
        int i,
        int j,
        GLuint a1,
        GLuint b1,
        vector<vec3> &pointsToWrite,
        vector<vec3> &normalsToWrite,
        vector<GLuint> &facesToWrite,
        vector<vector<vec3>> &normalsBesideToWrite,
        vector<float> &onEdgeToWrite);

#endif // VBOMESH_H
