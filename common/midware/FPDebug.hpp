/**
 * @file FPDebug.hpp
 * 
 * @author Sijia Rong (sijia.rong@fitplay.co)
 * @brief Fitplay debug script
 * @version 0.1
 * @date 2022-11-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef FPDEBUG_HPP_
#define FPDEBUG_HPP_

#include "FPBody.hpp"
#include <vector>

namespace fitplay {

class FPDebug
{
    
private:
vector<FPBody*>* _body_vec; 

public:
    FPDebug();
    ~FPDebug();

    const bool AddBody(FPBody* body);
    const vec3 GetStd(int num);
    const vec3 GetMin(int num);
    const vec3 GetMax(int num);
    const bool AddBody();
    const int Clean();
};

FPDebug::FPDebug()
{
   _body_vec = new vector<FPBody*>();
}

FPDebug::~FPDebug(){
    delete _body_vec;
}

const bool FPDebug::AddBody(FPBody* body){
    std::cout << __FUNCTION__ << ": Enter" << std::endl;
    _body_vec->push_back(body);
    return true;
}

const vec3 FPDebug::GetStd(int num){
    vec3 sum = vec3(0, 0, 0);
    for(auto body : *_body_vec){
        sum += body->GetJoint(num)->GetPosition();
    }
    vec3 mean = sum / (float)_body_vec->size();

    vec3 sq_sum = vec3(0, 0, 0);

    for(auto body : *_body_vec){
        vec3 pos = body->GetJoint(num)->GetPosition();
        vec3 delta = pos - mean;
        sq_sum += vec3(delta.x * delta.x, delta.y * delta.y, delta.z * delta.z);
    }

    return glm::sqrt(sq_sum / (float)_body_vec->size());
}

const vec3 FPDebug::GetMin(int num){
    vec3 min = vec3(MAXFLOAT, MAXFLOAT, MAXFLOAT);
    for(auto body : *_body_vec){
        vec3 pos = body->GetJoint(num)->GetPosition();
        if(pos.x < min.x){
            min.x = pos.x;
        }
        if(pos.y < min.y){
            min.y = pos.y;
        }
        if(pos.z < min.z){
            min.z = pos.z;
        }
    }
    
    return min;
}

const vec3 FPDebug::GetMax(int num){
    vec3 max = vec3(-MAXFLOAT, -MAXFLOAT, -MAXFLOAT);
    for(auto body : *_body_vec){
        vec3 pos = body->GetJoint(num)->GetPosition();
        if(pos.x > max.x){
            max.x = pos.x;
        }
        if(pos.y > max.y){
            max.y = pos.y;
        }
        if(pos.z > max.z){
            max.z = pos.z;
        }
    }
    
    return max;
}

const int FPDebug::Clean(){
    cout << "Enter: " << __FUNCTION__ << endl;
    for(auto body : *_body_vec){
        delete body;
    }
    _body_vec->clear();
    return 0;
}



}

#endif /* FPDEBUG_HPP_ */