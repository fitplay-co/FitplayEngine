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

std::vector<std::vector<vec3>> landmarkList;
std::vector<std::vector<vec3>> landmarkList3D;

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
        fstream.close();
    }
    return;
}

landmarks readLandmarks(int index) {
    landmarks landmarkData;
    landmarkData.head = landmarkList3D[index][0];
    landmarkData.lshoulder = landmarkList3D[index][11];
    landmarkData.rshoulder = landmarkList3D[index][12];
    landmarkData.larm = landmarkList3D[index][13];
    landmarkData.rarm = landmarkList3D[index][14];
    landmarkData.lwrist = landmarkList3D[index][15];
    landmarkData.rwrist = landmarkList3D[index][16];
    landmarkData.lhand = landmarkList3D[index][19];
    landmarkData.rhand = landmarkList3D[index][20];
    landmarkData.lhip = landmarkList3D[index][23];
    landmarkData.rhip = landmarkList3D[index][24];
    landmarkData.lknee = landmarkList3D[index][25];
    landmarkData.rknee = landmarkList3D[index][26];
    landmarkData.lankle = landmarkList3D[index][27];
    landmarkData.rankle = landmarkList3D[index][28];
    landmarkData.lfoot = landmarkList3D[index][31];
    landmarkData.rfoot = landmarkList3D[index][32];

    landmarkData.hipcenter = vec3(0.0f ,0.0f ,0.0f);
    landmarkData.neck = vec3((landmarkData.lshoulder[0] 
                + landmarkData.rshoulder[0])/2.0f, (landmarkData.lshoulder[1] 
                + landmarkData.rshoulder[1])/2.0f, (landmarkData.lshoulder[2] + landmarkData.rshoulder[2])/2.0f);
    
    return landmarkData;
}

#endif