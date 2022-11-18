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

    int UpdatePosition();

public:
    FPJoint();
    FPJoint(int id);
    FPJoint(vec3 bone_direction, float bone_len, FPJoint* p_parent, int id);
    ~FPJoint();

    vec3 GetPosition();
    int UpdateDirection(vec3 bone_direction, float confidence); /*x, y, z, confidence*/
};

FPJoint::FPJoint(){
    FPJoint(vec3(0, 0, 0), 0, NULL, -1);
}

FPJoint::FPJoint(int id){
    FPJoint(vec3(0, 0, 0), 0, NULL, id);
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
    // std::cout << __FUNCTION__ << ": Enter id: " << this->_id << std::endl;

    // std::cout << __FUNCTION__ << " position: " 
    // << _position.x << ", "<< _position.y << ", " << _position.z << std::endl;

    // UpdatePosition(); Note: costly

    return this->_position;
}

int FPJoint::UpdateDirection(vec3 bone_direction, float confidence){
    // std::cout << __FUNCTION__ << ": Enter id: " << this->_id << std::endl;
    // std::cout << __FUNCTION__ << " position: " 
    // << bone_direction.x << ", "<< bone_direction.y << ", " << bone_direction.z << std::endl;
    if(confidence < 0.03){
        // std::cout << __FUNCTION__ << "unacceptable confidence: " << confidence << std::endl;
        return 1;
    }
    _bone_direction = glm::normalize(bone_direction);
    UpdatePosition();
    // TODO: add constrains
    return 0;
}
/* update in GetPosition if you want to update very joints on the chain, but that will cost much more computational power*/
int FPJoint::UpdatePosition(){ 
    // std::cout << __FUNCTION__ << ": Enter id: " << this->_id << std::endl;
    if(this->_p_parent != NULL){
        this->_position = this->_p_parent->GetPosition() + this->_bone_len * this->_bone_direction;
        return 0;
    }
    return -1;
}



}
#endif /* FPJOINT_HPP_ */
