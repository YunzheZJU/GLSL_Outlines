#include "vbomesh.h"
#include <fstream>
#include <sstream>

VBOMesh::VBOMesh(const char *fileName, bool center, bool loadTc, bool genTangents) :
        reCenterMesh(center), loadTex(loadTc), genTang(genTangents) {
    loadOBJ(fileName);
}

void VBOMesh::render() const {
    glBindVertexArray(vaoHandle);
    glDrawElements(GL_TRIANGLES, 3 * faces, GL_UNSIGNED_INT, ((GLubyte *) nullptr + (0)));
}

void VBOMesh::loadOBJ(const char *fileName) {
    vector<vec3> points;
    vector<vec3> normals;
    vector<vec2> texCoords;
    vector<GLuint> faces;
    vector<vector<vec3>> normalsBeside;
    vector<vec3> faceNormals;
    vector<float> onEdge;

    int nFaces = 0;

    ifstream objStream(fileName, std::ios::in);

    if (!objStream) {
        cerr << "Unable to open OBJ file: " << fileName << endl;
        exit(1);
    }

    string line, token;
    vector<int> face;

    getline(objStream, line);
    while (!objStream.eof()) {
        trimString(line);
        if (line.length() > 0 && line.at(0) != '#') {
            istringstream lineStream(line);

            lineStream >> token;

            if (token == "v") {
                float x, y, z;
                lineStream >> x >> y >> z;
                points.emplace_back(x, y, z);
            } else if (token == "vt" && loadTex) {
                // Process texture coordinate
                float s, t;
                lineStream >> s >> t;
                texCoords.emplace_back(s, t);
            } else if (token == "vn") {
                float x, y, z;
                lineStream >> x >> y >> z;
                normals.emplace_back(x, y, z);
            } else if (token == "f") {
                nFaces++;

                // Process face
                face.clear();
                size_t slash1, slash2;
                //int point, texCoord, normal;
                while (lineStream.good()) {
                    string vertString;
                    lineStream >> vertString;
                    int pIndex, nIndex = -1, tcIndex = -1;

                    slash1 = vertString.find('/');
                    if (slash1 == string::npos) {
                        pIndex = strtol(vertString.c_str(), nullptr, 10) - 1;
                    } else {
                        slash2 = vertString.find('/', slash1 + 1);
                        pIndex = strtol(vertString.substr(0, slash1).c_str(), nullptr, 10) - 1;
                        if (slash2 > slash1 + 1) {
                            tcIndex = strtol(vertString.substr(slash1 + 1, slash2).c_str(), nullptr, 10) - 1;
                        }
                        nIndex = strtol(vertString.substr(slash2 + 1, vertString.length()).c_str(), nullptr, 10) - 1;
                    }
                    if (pIndex == -1) {
                        printf("Missing point index!!!");
                    } else {
                        face.push_back(pIndex);
                    }

                    if (loadTex && tcIndex != -1 && pIndex != tcIndex) {
                        printf("Texture and point indices are not consistent.\n");
                    }
                    if (nIndex != -1 && nIndex != pIndex) {
                        printf("Normal and point indices are not consistent.\n");
                    }
                }
                // If number of edges in face is greater than 3,
                // decompose into triangles as a triangle fan.
                if (face.size() > 3) {
                    int v0 = face[0];
                    int v1 = face[1];
                    int v2 = face[2];
                    // First face
                    faces.push_back(static_cast<GLuint>(v0));
                    faces.push_back(static_cast<GLuint>(v1));
                    faces.push_back(static_cast<GLuint>(v2));
                    for (GLuint i = 3; i < face.size(); i++) {
                        v1 = v2;
                        v2 = face[i];
                        faces.push_back(static_cast<GLuint>(v0));
                        faces.push_back(static_cast<GLuint>(v1));
                        faces.push_back(static_cast<GLuint>(v2));
                    }
                } else {
                    faces.push_back(static_cast<GLuint>(face[0]));
                    faces.push_back(static_cast<GLuint>(face[1]));
                    faces.push_back(static_cast<GLuint>(face[2]));
                }
            }
        }
        getline(objStream, line);
    }

    objStream.close();

    if (normals.empty()) {
        generateAveragedNormals(points, normals, faces);
    }

    vector<vec4> tangents;
    if (genTang && !texCoords.empty()) {
        generateTangents(points, normals, faces, texCoords, tangents);
    }

    if (reCenterMesh) {
        center(points);
    }

    cout << "Generating face normals" << endl;
    generateNormals(points, faces, faceNormals);

    cout << "Adding quads" << endl;
    addQuads(points, normals, faces, normalsBeside, onEdge, faceNormals);

    storeVBO(points, normals, texCoords, tangents, faces, normalsBeside, onEdge);

    cout << "Loaded mesh from: " << fileName << endl;
    cout << " " << points.size() << " points" << endl;
    cout << " " << nFaces << " original faces" << endl;
    cout << " " << faces.size() / 3 << " triangles." << endl;
    cout << " " << normals.size() << " normals" << endl;
    cout << " " << tangents.size() << " tangents " << endl;
    cout << " " << texCoords.size() << " texture coordinates." << endl;
}

void VBOMesh::center(vector<vec3> &points) {
    if (points.empty()) return;

    vec3 maxPoint = points[0];
    vec3 minPoint = points[0];

    // Find the AABB
    for (auto &point : points) {
        if (point.x > maxPoint.x) maxPoint.x = point.x;
        if (point.y > maxPoint.y) maxPoint.y = point.y;
        if (point.z > maxPoint.z) maxPoint.z = point.z;
        if (point.x < minPoint.x) minPoint.x = point.x;
        if (point.y < minPoint.y) minPoint.y = point.y;
        if (point.z < minPoint.z) minPoint.z = point.z;
    }

    // Center of the AABB
    vec3 center = vec3((maxPoint.x + minPoint.x) / 2.0f,
                       (maxPoint.y + minPoint.y) / 2.0f,
                       (maxPoint.z + minPoint.z) / 2.0f);

    // Translate center of the AABB to the origin
    for (auto &point : points) {
        point = point - center;
    }
}

void VBOMesh::generateAveragedNormals(
        const vector<vec3> &points,
        vector<vec3> &normals,
        const vector<GLuint> &faces) {
    for (GLuint i = 0; i < points.size(); i++) {
        normals.emplace_back(0.0f);
    }

    for (GLuint i = 0; i < faces.size(); i += 3) {
        const vec3 &p1 = points[faces[i]];
        const vec3 &p2 = points[faces[i + 1]];
        const vec3 &p3 = points[faces[i + 2]];

        vec3 a = p2 - p1;
        vec3 b = p3 - p1;
        vec3 n = glm::normalize(glm::cross(a, b));

        normals[faces[i]] += n;
        normals[faces[i + 1]] += n;
        normals[faces[i + 2]] += n;
    }

    for (auto &normal : normals) {
        normal = glm::normalize(normal);
    }
}

void VBOMesh::generateNormals(
        const vector<vec3> &points,
        const vector<GLuint> &faces,
        vector<vec3> &faceNormals) {

    for (GLuint i = 0; i < faces.size(); i += 3) {
        const vec3 &p1 = points[faces[i]];
        const vec3 &p2 = points[faces[i + 1]];
        const vec3 &p3 = points[faces[i + 2]];

        vec3 a = p2 - p1;
        vec3 b = p3 - p1;
        vec3 n = glm::normalize(glm::cross(a, b));

        faceNormals.push_back(n);
    }
}

void VBOMesh::generateTangents(
        const vector<vec3> &points,
        const vector<vec3> &normals,
        const vector<GLuint> &faces,
        const vector<vec2> &texCoords,
        vector<vec4> &tangents) {
    vector<vec3> tan1Accum;
    vector<vec3> tan2Accum;

    for (GLuint i = 0; i < points.size(); i++) {
        tan1Accum.emplace_back(0.0f);
        tan2Accum.emplace_back(0.0f);
        tangents.emplace_back(0.0f);
    }

    // Compute the tangent vector
    for (GLuint i = 0; i < faces.size(); i += 3) {
        const vec3 &p1 = points[faces[i]];
        const vec3 &p2 = points[faces[i + 1]];
        const vec3 &p3 = points[faces[i + 2]];

        const vec2 &tc1 = texCoords[faces[i]];
        const vec2 &tc2 = texCoords[faces[i + 1]];
        const vec2 &tc3 = texCoords[faces[i + 2]];

        vec3 q1 = p2 - p1;
        vec3 q2 = p3 - p1;
        float s1 = tc2.x - tc1.x, s2 = tc3.x - tc1.x;
        float t1 = tc2.y - tc1.y, t2 = tc3.y - tc1.y;
        float r = 1.0f / (s1 * t2 - s2 * t1);
        vec3 tan1((t2 * q1.x - t1 * q2.x) * r,
                  (t2 * q1.y - t1 * q2.y) * r,
                  (t2 * q1.z - t1 * q2.z) * r);
        vec3 tan2((s1 * q2.x - s2 * q1.x) * r,
                  (s1 * q2.y - s2 * q1.y) * r,
                  (s1 * q2.z - s2 * q1.z) * r);
        tan1Accum[faces[i]] += tan1;
        tan1Accum[faces[i + 1]] += tan1;
        tan1Accum[faces[i + 2]] += tan1;
        tan2Accum[faces[i]] += tan2;
        tan2Accum[faces[i + 1]] += tan2;
        tan2Accum[faces[i + 2]] += tan2;
    }

    for (GLuint i = 0; i < points.size(); ++i) {
        const vec3 &n = normals[i];
        vec3 &t1 = tan1Accum[i];
        vec3 &t2 = tan2Accum[i];

        // Gram-Schmidt orthogonalize
        tangents[i] = vec4(glm::normalize(t1 - (glm::dot(n, t1) * n)), 0.0f);
        // Store handedness in w
        tangents[i].w = (glm::dot(glm::cross(n, t1), t2) < 0.0f) ? -1.0f : 1.0f;
    }
    tan1Accum.clear();
    tan2Accum.clear();
}

void VBOMesh::storeVBO(const vector<vec3> &points,
                       const vector<vec3> &normals,
                       const vector<vec2> &texCoords,
                       const vector<vec4> &tangents,
                       const vector<GLuint> &elements,
                       const vector<vector<vec3>> &normalsBeside,
                       const vector<float> &onEdge) {
    GLuint nVerts = points.size();
    faces = elements.size() / 3;

    auto *v = new float[3 * nVerts];
    auto *n = new float[3 * nVerts];
    float *tc = nullptr;
    float *tang = nullptr;
    int nfn = 0;
    auto *fn = new float[9 * 3 * nVerts];
    auto *e = new float[nVerts];

    if (!texCoords.empty()) {
        tc = new float[2 * nVerts];
        if (!tangents.empty())
            tang = new float[4 * nVerts];
    }

    auto *el = new unsigned int[elements.size()];

    int idx = 0, tcIdx = 0, tangIdx = 0;
    for (GLuint i = 0; i < nVerts; ++i) {
        v[idx] = points[i].x;
        v[idx + 1] = points[i].y;
        v[idx + 2] = points[i].z;
        n[idx] = normals[i].x;
        n[idx + 1] = normals[i].y;
        n[idx + 2] = normals[i].z;
        e[i] = onEdge[i];
        nfn = normalsBeside[i].size();
        for (int j = 0; j < nfn; j++) {
            fn[j * 3 * nVerts + idx] = normalsBeside[i][j].x;
            fn[j * 3 * nVerts + idx + 1] = normalsBeside[i][j].y;
            fn[j * 3 * nVerts + idx + 2] = normalsBeside[i][j].z;
        }
        idx += 3;
        if (tc != nullptr) {
            tc[tcIdx] = texCoords[i].x;
            tc[tcIdx + 1] = texCoords[i].y;
            tcIdx += 2;
        }
        if (tang != nullptr) {
            tang[tangIdx] = tangents[i].x;
            tang[tangIdx + 1] = tangents[i].y;
            tang[tangIdx + 2] = tangents[i].z;
            tang[tangIdx + 3] = tangents[i].w;
            tangIdx += 4;
        }
    }
    for (unsigned int i = 0; i < elements.size(); ++i) {
        el[i] = elements[i];
    }
    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

    int nBuffers = 13;
    if (tc != nullptr) nBuffers++;
    if (tang != nullptr) nBuffers++;

    GLuint handle[15];
    glGenBuffers(nBuffers, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, (3 * nVerts) * sizeof(float), v, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint) 0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *) nullptr + (0)));
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, (3 * nVerts) * sizeof(float), n, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint) 1, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *) nullptr + (0)));
    glEnableVertexAttribArray(1);  // Vertex normal

    for (int i = 0; i < 2; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, handle[2 + i]);
        glBufferData(GL_ARRAY_BUFFER, (3 * nVerts) * sizeof(float), fn + i * 3 * nVerts, GL_STATIC_DRAW);
        glVertexAttribPointer((GLuint) 2 + i, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *) nullptr + (0)));
        glEnableVertexAttribArray(static_cast<GLuint>(2 + i));  // Face normal i
    }

    glBindBuffer(GL_ARRAY_BUFFER, handle[4]);
    glBufferData(GL_ARRAY_BUFFER, nVerts * sizeof(float), e, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint) 4, 1, GL_FLOAT, GL_FALSE, 0, ((GLubyte *) nullptr + (0)));
    glEnableVertexAttribArray(4);  // On Edge

    if (tc != nullptr) {
        glBindBuffer(GL_ARRAY_BUFFER, handle[5]);
        glBufferData(GL_ARRAY_BUFFER, (2 * nVerts) * sizeof(float), tc, GL_STATIC_DRAW);
        glVertexAttribPointer((GLuint) 5, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte *) nullptr + (0)));
        glEnableVertexAttribArray(5);  // Texture coords
    }
    if (tang != nullptr) {
        glBindBuffer(GL_ARRAY_BUFFER, handle[6]);
        glBufferData(GL_ARRAY_BUFFER, (4 * nVerts) * sizeof(float), tang, GL_STATIC_DRAW);
        glVertexAttribPointer((GLuint) 6, 4, GL_FLOAT, GL_FALSE, 0, ((GLubyte *) nullptr + (0)));
        glEnableVertexAttribArray(6);  // Tangent vector
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[nBuffers - 1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * faces * sizeof(unsigned int), el, GL_STATIC_DRAW);

    glBindVertexArray(0);

    // Clean up
    delete[] v;
    delete[] n;
    delete[] tc;
    delete[] tang;
    delete[] el;
    delete[] fn;
    delete[] e;
    printf("End storeVBO\n");
}

void VBOMesh::trimString(string &str) {
    const char *whiteSpace = " \t\n\r";
    size_t location;
    location = str.find_first_not_of(whiteSpace);
    str.erase(0, location);
    location = str.find_last_not_of(whiteSpace);
    str.erase(location + 1);
}

void VBOMesh::addQuads(
        vector<vec3> &points,
        vector<vec3> &normals,
        vector<GLuint> &faces,
        vector<vector<vec3>> &normalsBeside,
        vector<float> &onEdge,
        vector<vec3> faceNormals) {
    vector<vec3> newVec;
    normalsBeside.insert(normalsBeside.begin(), points.size(), newVec);
    vector<GLuint> facesToAdd;

    // Initialize onEdge
    onEdge.insert(onEdge.begin(), points.size(), 0.0);

    for (int i = 0; i < faces.size(); i += 3) {
        GLuint a1 = faces[i];
        GLuint b1 = faces[i + 1];
        GLuint c1 = faces[i + 2];

        if (i % 1000 == 0) {
            cout << i << endl;
        }

        for (int j = i + 3; j < faces.size(); j += 3) {
            GLuint a2 = faces[j];
            GLuint b2 = faces[j + 1];
            GLuint c2 = faces[j + 2];

            // Edge 1 == Edge 1
            if ((a1 == a2 && b1 == b2) || (a1 == b2 && b1 == a2)) {
                addSingleQuad(a1, b1, points, normals, facesToAdd, normalsBeside, onEdge, faceNormals, i / 3, j / 3);
            }
                // or Edge 1 == Edge 2
            else if ((a1 == b2 && b1 == c2) || (a1 == c2 && b1 == b2)) {
                addSingleQuad(a1, b1, points, normals, facesToAdd, normalsBeside, onEdge, faceNormals, i / 3, j / 3);
            }
                // or Edge 1 == Edge 3
            else if ((a1 == c2 && b1 == a2) || (a1 == a2 && b1 == c2)) {
                addSingleQuad(a1, b1, points, normals, facesToAdd, normalsBeside, onEdge, faceNormals, i / 3, j / 3);
            }
            // Edge 2 == Edge 1
            if ((b1 == a2 && c1 == b2) || (b1 == b2 && c1 == a2)) {
                addSingleQuad(b1, c1, points, normals, facesToAdd, normalsBeside, onEdge, faceNormals, i / 3, j / 3);
            }
                // or Edge 2 == Edge 2
            else if ((b1 == b2 && c1 == c2) || (b1 == c2 && c1 == b2)) {
                addSingleQuad(b1, c1, points, normals, facesToAdd, normalsBeside, onEdge, faceNormals, i / 3, j / 3);
            }
                // or Edge 2 == Edge 3
            else if ((b1 == c2 && c1 == a2) || (b1 == a2 && c1 == c2)) {
                addSingleQuad(b1, c1, points, normals, facesToAdd, normalsBeside, onEdge, faceNormals, i / 3, j / 3);
            }
            // Edge 3 == Edge 1
            if ((c1 == a2 && a1 == b2) || (c1 == b2 && a1 == a2)) {
                addSingleQuad(c1, a1, points, normals, facesToAdd, normalsBeside, onEdge, faceNormals, i / 3, j / 3);
            }
                // or Edge 3 == Edge 2
            else if ((c1 == b2 && a1 == c2) || (c1 == c2 && a1 == b2)) {
                addSingleQuad(c1, a1, points, normals, facesToAdd, normalsBeside, onEdge, faceNormals, i / 3, j / 3);
            }
                // or Edge 3 == Edge 3
            else if ((c1 == c2 && a1 == a2) || (c1 == a2 && a1 == c2)) {
                addSingleQuad(c1, a1, points, normals, facesToAdd, normalsBeside, onEdge, faceNormals, i / 3, j / 3);
            }
        }
    }
    // Append faces
    faces.insert(faces.end(), facesToAdd.begin(), facesToAdd.end());
}

void VBOMesh::addSingleQuad(
        GLuint a1,
        GLuint b1,
        vector<vec3> &points,
        vector<vec3> &normals,
        vector<GLuint> &facesToAdd,
        vector<vector<vec3>> &faceNormals,
        vector<float> &onEdge,
        vector<vec3> &faceNormal,
        int i,
        int j) {
    int pointsSize = points.size();
    // Add points
    points.push_back(points[a1]);
    points.push_back(points[b1]);
    points.push_back(points[b1]);
    points.push_back(points[b1]);
    points.push_back(points[a1]);
    points.push_back(points[a1]);
    // Add normals
    normals.push_back(normals[a1]);
    normals.push_back(normals[b1]);
    normals.push_back(normals[b1]);
    normals.push_back(normals[b1]);
    normals.push_back(normals[a1]);
    normals.push_back(normals[a1]);
    // Add faces
    facesToAdd.push_back(static_cast<GLuint>(pointsSize));
    facesToAdd.push_back(static_cast<GLuint>(pointsSize + 1));
    facesToAdd.push_back(static_cast<GLuint>(pointsSize + 2));
    facesToAdd.push_back(static_cast<GLuint>(pointsSize + 3));
    facesToAdd.push_back(static_cast<GLuint>(pointsSize + 4));
    facesToAdd.push_back(static_cast<GLuint>(pointsSize + 5));
    // Add face normals
    vector<vec3> newVec_empty;
    vector<vec3> newVec = {faceNormal[i], faceNormal[j]};
    faceNormals.push_back(newVec_empty);
    faceNormals.push_back(newVec_empty);
    faceNormals.push_back(newVec);
    faceNormals.push_back(newVec);
    faceNormals.push_back(newVec);
    faceNormals.push_back(newVec_empty);
    // Add onEdge
    onEdge.insert(onEdge.end(), 6, 1.0);
}
