//
// Created by Yunzhe on 2018/1/7.
//

#ifndef GPUBASEDRENDERING_A2_THREADS_H
#define GPUBASEDRENDERING_A2_THREADS_H

// Include related head files
#include "global.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

// Using namespace std for cout
using namespace std;

extern mutex mtx;

void create(
        int start,
        int end,
        vector<vec3> &pointsMaster
//        vector<vec3> &normalsMaster
//        vector<GLuint> &facesMaster,
//        vector<vector<vec3>> &normalsBesideMaster,
//        vector<float> &onEdgeMaster,
//        vector<vec3> &faceNormalsMaster
);

void fuck (int i, int j, int k);

void addSingleQuad(
        int i,
        int j,
        GLuint a1,
        GLuint b1,
        vector<vec3> &pointsToWrite,
        vector<vec3> &normalsToWrite,
        vector<GLuint> &facesToWrite,
        vector<vector<vec3>> &normalsBesideToWrite,
        vector<float> &onEdgeToWrite,
        const vector<vec3> &pointsMaster,
        const vector<vec3> &normalsMaster,
        const vector<vec3> &faceNormalsMaster);

#endif //GPUBASEDRENDERING_A2_THREADS_H
