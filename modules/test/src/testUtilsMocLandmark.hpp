#ifndef FITPLAY_testuitlsmocklandmark
#define FITPLAY_testuitlsmocklandmark

#include <iostream>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "testUtils.hpp"
#include "fitting/fitting.hpp"

using namespace std;
using namespace glm;
using namespace fitplay;

std::vector<landmarks> landmarkList;
std::vector<landmarks> landmarkList3D;

std::vector<std::string> stringSplit(const std::string& str, char delim) {
    std::size_t previous = 0;
    std::size_t current = str.find(delim);
    std::vector<std::string> elems;
    while (current != std::string::npos) {
        if (current > previous) {
            elems.push_back(str.substr(previous, current - previous));
        }
        previous = current + 1;
        current = str.find(delim, previous);
    }
    if (previous != str.size()) {
        elems.push_back(str.substr(previous));
    }
    return elems;
}

void readCsvFileAsLandmarkList(std::string path, int limit = 1000) {
    cout << "read landmarks " << path << endl;
    std::ifstream fstream(path, ios::in);
    int n = 0;
    int jointIndex = 0;
    int readingStatus;
    std::vector<vec3> currentRes;
    std::string tmp;
    landmarkList.clear();
    landmarkList3D.clear();
    if(fstream.fail()) {
        cout << "failed" << endl;
    } else {
        while (getline(fstream, tmp, '\n')) {
            if(tmp.rfind("Frame", 0) == 0) {
                n++;
                readingStatus = 0;
                if(n > limit) {
                    return;
                }
            } else if(tmp.rfind("keyPoints3D", 0) == 0) {
                readingStatus = 2;
                jointIndex = 0;
            } else if(tmp.rfind("keyPoints", 0) ==0) {
                readingStatus = 1;
                jointIndex = 0;
            } else {
                std::vector<string> tempSplited = stringSplit(tmp, ',');
                vec3 v = vec3(std::stof(tempSplited[1]), std::stof(tempSplited[2]), std::stof(tempSplited[3]));
                currentRes.push_back(v);
                jointIndex++;
            }
            if(jointIndex == 33) {
                switch (readingStatus) {
                    case 1: 
                        landmarkList.push_back(std::vector<vec3>(currentRes));
                        currentRes.clear();
                        break;
                    case 2:
                        landmarkList3D.push_back(std::vector<vec3>(currentRes));
                        currentRes.clear();
                    default:
                        break;
                }
            }
        }
        cout << "end read landmarks " << path << endl;
        fstream.close();
    }
    return;
}

landmarks readLandmarks(int index) {
    landmarks landmarkData(jointPointSize + 1);

    landmarkData[HEAD] = landmarkList3D[index][0];
    landmarkData[L_SHOULDER] = landmarkList3D[index][11];
    landmarkData[R_SHOULDER] = landmarkList3D[index][12];
    landmarkData[L_ARM] = landmarkList3D[index][13];
    landmarkData[R_ARM] = landmarkList3D[index][14];
    landmarkData[L_WRIST] = landmarkList3D[index][15];
    landmarkData[R_WRIST] = landmarkList3D[index][16];
    landmarkData[L_HAND] = landmarkList3D[index][19];
    landmarkData[R_HAND] = landmarkList3D[index][20];
    landmarkData[L_HIP] = landmarkList3D[index][23];
    landmarkData[R_HIP] = landmarkList3D[index][24];
    landmarkData[L_KNEE] = landmarkList3D[index][25];
    landmarkData[R_KNEE] = landmarkList3D[index][26];
    landmarkData[L_ANKLE] = landmarkList3D[index][27];
    landmarkData[R_ANKLE] = landmarkList3D[index][28];
    landmarkData[L_FOOT] = landmarkList3D[index][31];
    landmarkData[R_FOOT] = landmarkList3D[index][32];

    landmarkData[HIP_CENTER] = vec3(0.0f ,0.0f ,0.0f);
    landmarkData[NECK] = vec3((landmarkData[L_SHOULDER][0] 
                + landmarkData[R_SHOULDER][0])/2.0f, (landmarkData[L_SHOULDER][1] 
                + landmarkData[R_SHOULDER][1])/2.0f, (landmarkData[L_SHOULDER][2] + landmarkData[R_SHOULDER][2])/2.0f);
    return landmarkData;
}

#endif