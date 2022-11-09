/**
 * @file FPJoint.hpp
 * 
 * @author Sijia Rong (sijia.rong@fitplay.co)
 * @brief 
 * @version 0.1
 * @date 2022-11-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef FPJOINT_HPP_
#define FPJOINT_HPP_

#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

using namespace glm;

#include "FPLandmarks.h"

namespace fitplay {

class FPJoint
{
private:
    FPJoint* _p_parent = NULL;
    vector<FPJoint*> _children = vector<FPJoint*>();

    int _id = -1;

    /* parent to this */
    float _bone_len = 1.0f; 
    vec3 _bone_direction = vec3(0.0f, 0.0f, 0.0f);
    
    vec3 _position = vec3(0.0f, 0.0f, 0.0f);

public:
    FPJoint();
    FPJoint(vec3 bone_direction, float bone_len, FPJoint* p_parent, int id);
    ~FPJoint();

    vec3 GetPosition();
    vec3 UpdatePosition();
};

FPJoint::FPJoint(){
    FPJoint(vec3(0, 0, 0), 0, NULL, -1);
}

FPJoint::FPJoint(vec3 bone_direction, float bone_len, FPJoint* p_parent, int id)
{
    this->_bone_direction = glm::normalize(bone_direction);
    // this->_bone_direction = bone_direction;
    this->_bone_len = bone_len * 1.3f/*better looking*/;
    this->_p_parent = p_parent;
    this->_id = id;

    UpdatePosition();

    if(_p_parent != NULL){
        _p_parent->_children.push_back(this);
    }
}

FPJoint::~FPJoint()
{
    // std::cout << __FUNCTION__ <<": id: " << this->_id << std::endl;
    // new/alloc nothing 
}

vec3 FPJoint::GetPosition(){
    // std::cout << __FUNCTION__ << ": Enter" << std::endl;
    // std::cout << __FUNCTION__ << ": id: " << _id << " position: " 
    // << _position.x << ", "<< _position.y << ", " << _position.z << std::endl;
    return this->_position;
}

vec3 FPJoint::UpdatePosition(){
    // std::cout << __FUNCTION__ << ": Enter" << std::endl;

    if(this->_p_parent != NULL){
        this->_position = this->_p_parent->GetPosition() + this->_bone_len * this->_bone_direction;
        // this->_position = this->_p_parent->GetPosition() + this->_bone_direction;
    }
    
    // std::cout << __FUNCTION__ <<": id: "<< _id << "; position: " 
    // << _position.x << ", "<< _position.y << ", " << _position.z << std::endl;
    // TODO: update children position
    return this->_position;
}

}
#endif /* FPJOINT_HPP_ */
