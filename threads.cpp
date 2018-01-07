//
// Created by Yunzhe on 2018/1/7.
//

#include "threads.h"

mutex mtx;

void fuck (int i, int j, int k) {

}

void create1(
        int start,
        int end,
        vector<vec3> &pointsMaster,
        vector<vec3> &normalsMaster,
        vector<GLuint> &facesMaster,
        vector<vector<vec3>> &normalsBesideMaster,
        vector<float> &onEdgeMaster,
        vector<vec3> &faceNormalsMaster) {
    vector<vec3> points;
    vector<vec3> normals;
    vector<GLuint> faces;
    vector<vector<vec3>> normalsBeside;
    vector<float> onEdge;
    for (int i = start; i < end; i += 3) {
        GLuint a1 = facesMaster[i];
        GLuint b1 = facesMaster[i + 1];
        GLuint c1 = facesMaster[i + 2];
        int _i = i / 3;
        bool flag_ab = false;
        bool flag_bc = false;
        bool flag_ca = false;

        if (_i % 1000 == 0) {
            cout << _i << endl;
        }


        for (int j = 0; j < facesMaster.size(); j += 3) {
            GLuint a2 = facesMaster[j];
            GLuint b2 = facesMaster[j + 1];
            GLuint c2 = facesMaster[j + 2];
            int _j = j / 3;

            if (i == j) {
                continue;
            }

            // Edge 1 == Edge 1
            if ((a1 == a2 && b1 == b2) || (a1 == b2 && b1 == a2)) {
                if (i < j) {
                    addSingleQuad(_i, _j, a1, b1, points, normals, faces, normalsBeside, onEdge, pointsMaster,
                                  normalsMaster, faceNormalsMaster);
                }
                flag_ab = true;
            }
                // or Edge 1 == Edge 2
            else if ((a1 == b2 && b1 == c2) || (a1 == c2 && b1 == b2)) {
                if (i < j) {
                    addSingleQuad(_i, _j, a1, b1, points, normals, faces, normalsBeside, onEdge, pointsMaster,
                                  normalsMaster, faceNormalsMaster);
                }
                flag_ab = true;
            }
                // or Edge 1 == Edge 3
            else if ((a1 == c2 && b1 == a2) || (a1 == a2 && b1 == c2)) {
                if (i < j) {
                    addSingleQuad(_i, _j, a1, b1, points, normals, faces, normalsBeside, onEdge, pointsMaster,
                                  normalsMaster, faceNormalsMaster);
                }
                flag_ab = true;
            } else {
                // Edge 2 == Edge 1
                if ((b1 == a2 && c1 == b2) || (b1 == b2 && c1 == a2)) {
                    if (i < j) {
                        addSingleQuad(_i, _j, b1, c1, points, normals, faces, normalsBeside, onEdge, pointsMaster,
                                      normalsMaster, faceNormalsMaster);
                    }
                    flag_bc = true;
                }
                    // or Edge 2 == Edge 2
                else if ((b1 == b2 && c1 == c2) || (b1 == c2 && c1 == b2)) {
                    if (i < j) {
                        addSingleQuad(_i, _j, b1, c1, points, normals, faces, normalsBeside, onEdge, pointsMaster,
                                      normalsMaster, faceNormalsMaster);
                    }
                    flag_bc = true;
                }
                    // or Edge 2 == Edge 3
                else if ((b1 == c2 && c1 == a2) || (b1 == a2 && c1 == c2)) {
                    if (i < j) {
                        addSingleQuad(_i, _j, b1, c1, points, normals, faces, normalsBeside, onEdge, pointsMaster,
                                      normalsMaster, faceNormalsMaster);
                    }
                    flag_bc = true;
                } else {
                    // Edge 3 == Edge 1
                    if ((c1 == a2 && a1 == b2) || (c1 == b2 && a1 == a2)) {
                        if (i < j) {
                            addSingleQuad(_i, _j, c1, a1, points, normals, faces, normalsBeside, onEdge, pointsMaster,
                                          normalsMaster, faceNormalsMaster);
                        }
                        flag_ca = true;
                    }
                        // or Edge 3 == Edge 2
                    else if ((c1 == b2 && a1 == c2) || (c1 == c2 && a1 == b2)) {
                        if (i < j) {
                            addSingleQuad(_i, _j, c1, a1, points, normals, faces, normalsBeside, onEdge, pointsMaster,
                                          normalsMaster, faceNormalsMaster);
                        }
                        flag_ca = true;
                    }
                        // or Edge 3 == Edge 3
                    else if ((c1 == c2 && a1 == a2) || (c1 == a2 && a1 == c2)) {
                        if (i < j) {
                            addSingleQuad(_i, _j, c1, a1, points, normals, faces, normalsBeside, onEdge, pointsMaster,
                                          normalsMaster, faceNormalsMaster);
                        }
                        flag_ca = true;
                    }
                }
            }
        }
        if (!flag_ab) {
            addSingleQuad(-1, -1, a1, b1, points, normals, faces, normalsBeside, onEdge, pointsMaster, normalsMaster,
                          faceNormalsMaster);
        }
        if (!flag_bc) {
            addSingleQuad(-1, -1, b1, c1, points, normals, faces, normalsBeside, onEdge, pointsMaster, normalsMaster,
                          faceNormalsMaster);
        }
        if (!flag_ca) {
            addSingleQuad(-1, -1, c1, a1, points, normals, faces, normalsBeside, onEdge, pointsMaster, normalsMaster,
                          faceNormalsMaster);
        }
    }

    // Merge to master
    mtx.lock();
    pointsMaster.insert(pointsMaster.end(), points.begin(), points.end());
    normalsMaster.insert(normalsMaster.end(), normals.begin(), normals.end());
    facesMaster.insert(facesMaster.end(), faces.begin(), faces.end());
    normalsBesideMaster.insert(normalsBesideMaster.end(), normalsBeside.begin(), normalsBeside.end());
    onEdgeMaster.insert(onEdgeMaster.end(), onEdge.begin(), onEdge.end());
    mtx.unlock();
}

void addSingleQuad(
        int i,
        int j,
        GLuint a1,
        GLuint b1,
        vector<vec3> &points,
        vector<vec3> &normals,
        vector<GLuint> &faces,
        vector<vector<vec3>> &normalsBeside,
        vector<float> &onEdge,
        const vector<vec3> &pointsMaster,
        const vector<vec3> &normalsMaster,
        const vector<vec3> &faceNormalsMaster) {
    int pointsSize = static_cast<int>(points.size());
    // Add points
    points.push_back(pointsMaster[a1]);
    points.push_back(pointsMaster[b1]);
    points.push_back(pointsMaster[b1]);
    points.push_back(pointsMaster[b1]);
    points.push_back(pointsMaster[a1]);
    points.push_back(pointsMaster[a1]);
    // Add normals
    normals.push_back(normalsMaster[a1]);
    normals.push_back(normalsMaster[b1]);
    normals.push_back(normalsMaster[b1]);
    normals.push_back(normalsMaster[b1]);
    normals.push_back(normalsMaster[a1]);
    normals.push_back(normalsMaster[a1]);
    // Add faces
    faces.push_back(static_cast<GLuint>(pointsSize));
    faces.push_back(static_cast<GLuint>(pointsSize + 1));
    faces.push_back(static_cast<GLuint>(pointsSize + 2));
    faces.push_back(static_cast<GLuint>(pointsSize + 3));
    faces.push_back(static_cast<GLuint>(pointsSize + 4));
    faces.push_back(static_cast<GLuint>(pointsSize + 5));
    // Add face normals
    vector<vec3> newVec_empty;
    vector<vec3> newVec = {faceNormalsMaster[i], faceNormalsMaster[j]};
    if (i == -1 && j == -1) {
        newVec = {vec3(2.0), vec3(2.0)};
    }
    normalsBeside.push_back(newVec_empty);
    normalsBeside.push_back(newVec_empty);
    normalsBeside.push_back(newVec);
    normalsBeside.push_back(newVec);
    normalsBeside.push_back(newVec);
    normalsBeside.push_back(newVec_empty);
    // Add onEdge
    onEdge.insert(onEdge.end(), 6, 1.0);
}

void create(
        int start,
        int end,
        vector<vec3> &pointsMaster
//        vector<vec3> &normalsMaster,
//        vector<GLuint> &facesMaster,
//        vector<vector<vec3>> &normalsBesideMaster,
//        vector<float> &onEdgeMaster,
//        vector<vec3> &faceNormalsMaster
    ) {

}
