#ifndef FITPLAY_testGlmLookat
#define FITPLAY_testGlmLookat

#include <iostream>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "testUtils.hpp"

using namespace std;
using namespace glm;

///https://www.andre-gaschler.com/rotationconverter/ 
// using eular Y Z X

/**
 * @brief 
 * 
 * print color text with
 * 
 * 
printf("\x1b[32m green text on regular background \x1b[0m  \n");
printf("\x1b[32;40m green text on black background \x1b[0m  \n");
printf("\x1b[42m regular text on green background \x1b[0m  \n");

+---------+------------+------------+
|  color  | foreground | background |
|         |    code    |    code    |
+---------+------------+------------+
| black   |     30     |     40     |
| red     |     31     |     41     |
| green   |     32     |     42     |
| yellow  |     33     |     43     |
| blue    |     34     |     44     |
| magenta |     35     |     45     |
| cyan    |     36     |     46     |
| white   |     37     |     47     |
+---------+------------+------------+
 * 
 * @param detailPrint 
 */
void testGlmLookat(bool detailPrint = false){
    cout << "starting glm lookat testing." << 1 << endl ;

    if(!detailPrint) {
        cout << "skip glm lookat testing." << 1 << endl ;
        return;
    }

    cout << "___________________________________________________" << endl;

    cout << "right hand direction left , upward ,up  " << endl; 
    quat q = glm::quatLookAtRH(testleft, testup);
    cout << "quat w: " << q.w << " x " << q.x << " y " << q.y << " z " << q.z << endl;
    
    vec3 rotatedX = q * testright;
    vec3 rotatedY = q * testup;
    vec3 rotatedZ = q * testforward;
    cout << "rotated xaxis x " << rotatedX.x << " y " << rotatedX.y << " z " << rotatedX.z  << endl;
    cout << "rotated yaxis y " << rotatedY.x << " y " << rotatedY.y << " z " << rotatedY.z  << endl;
    cout << "rotated zaxis x " << rotatedZ.x << " y " << rotatedZ.y << " z " << rotatedZ.z  << endl;

    cout << "wrong rotation z" << endl; 

    cout << "___________________________________________________" << endl;

    cout << "left hand direction left , upward ,up  " << endl; 
    q = glm::quatLookAtLH(testleft, testup);
    cout << "quat w: " << q.w << " x " << q.x << " y " << q.y << " z " << q.z << endl;

    rotatedX = q * testright;
    rotatedY = q * testup;
    rotatedZ = q * testforward;
    cout << "rotated xaxis x " << rotatedX.x << " y " << rotatedX.y << " z " << rotatedX.z  << endl;
    cout << "rotated yaxis y " << rotatedY.x << " y " << rotatedY.y << " z " << rotatedY.z  << endl;
    cout << "rotated zaxis x " << rotatedZ.x << " y " << rotatedZ.y << " z " << rotatedZ.z  << endl;

    cout << "correct rotation z" << endl; 

    cout << "___________________________________________________" << endl;

    cout << "test direction (1, 0.1, 0), upward up" << endl;
    vec3 testDirection = vec3(1.0f, 0.1f, 0.0f);
    q = glm::quatLookAtLH(glm::normalize(testDirection), testup);
    cout << "quat w: " << q.w << " x " << q.x << " y " << q.y << " z " << q.z << endl;

    rotatedX = q * testright;
    rotatedY = q * testup;
    rotatedZ = q * testforward;
    cout << "rotated xaxis x " << rotatedX.x << " y " << rotatedX.y << " z " << rotatedX.z  << endl;
    cout << "rotated yaxis y " << rotatedY.x << " y " << rotatedY.y << " z " << rotatedY.z  << endl;
    cout << "rotated zaxis x " << rotatedZ.x << " y " << rotatedZ.y << " z " << rotatedZ.z  << endl;

    cout << "___________________________________________________" << endl;

    cout << "test direction (1, 0.5, 0), upward up" << endl;
    testDirection = vec3(1.0f, 0.5f, 0.0f);
    q = glm::quatLookAtLH(glm::normalize(testDirection), testup);
    cout << "quat w: " << q.w << " x " << q.x << " y " << q.y << " z " << q.z << endl;

    rotatedX = q * testright;
    rotatedY = q * testup;
    rotatedZ = q * testforward;
    cout << "rotated xaxis x " << rotatedX.x << " y " << rotatedX.y << " z " << rotatedX.z  << endl;
    cout << "rotated yaxis y " << rotatedY.x << " y " << rotatedY.y << " z " << rotatedY.z  << endl;
    cout << "rotated zaxis x " << rotatedZ.x << " y " << rotatedZ.y << " z " << rotatedZ.z  << endl;

    cout << "___________________________________________________" << endl;

    cout << "test direction (1, 1, 0), upward up" << endl;
    testDirection = vec3(1.0f, 1.0f, 0.0f);
    q = glm::quatLookAtLH(glm::normalize(testDirection), testup);
    cout << "quat w: " << q.w << " x " << q.x << " y " << q.y << " z " << q.z << endl;

    rotatedX = q * testright;
    rotatedY = q * testup;
    rotatedZ = q * testforward;
    cout << "rotated xaxis x " << rotatedX.x << " y " << rotatedX.y << " z " << rotatedX.z  << endl;
    cout << "rotated yaxis y " << rotatedY.x << " y " << rotatedY.y << " z " << rotatedY.z  << endl;
    cout << "rotated zaxis x " << rotatedZ.x << " y " << rotatedZ.y << " z " << rotatedZ.z  << endl;

    cout << "___________________________________________________" << endl;

    cout << "test direction (1, 0, 1), upward up" << endl;
    testDirection = vec3(1.0f, 0.0f, 1.0f);
    q = glm::quatLookAtLH(glm::normalize(testDirection), testup);
    cout << "quat w: " << q.w << " x " << q.x << " y " << q.y << " z " << q.z << endl;

    rotatedX = q * testright;
    rotatedY = q * testup;
    rotatedZ = q * testforward;
    cout << "rotated xaxis x " << rotatedX.x << " y " << rotatedX.y << " z " << rotatedX.z  << endl;
    cout << "rotated yaxis y " << rotatedY.x << " y " << rotatedY.y << " z " << rotatedY.z  << endl;
    cout << "rotated zaxis x " << rotatedZ.x << " y " << rotatedZ.y << " z " << rotatedZ.z  << endl;

    cout << "___________________________________________________" << endl;

    cout << "test direction (1, 0, -1), upward up" << endl;
    testDirection = vec3(1.0f, 0.0f, -1.0f);
    q = glm::quatLookAtLH(glm::normalize(testDirection), testup);
    cout << "quat w: " << q.w << " x " << q.x << " y " << q.y << " z " << q.z << endl;

    rotatedX = q * testright;
    rotatedY = q * testup;
    rotatedZ = q * testforward;
    cout << "rotated xaxis x " << rotatedX.x << " y " << rotatedX.y << " z " << rotatedX.z  << endl;
    cout << "rotated yaxis y " << rotatedY.x << " y " << rotatedY.y << " z " << rotatedY.z  << endl;
    cout << "rotated zaxis x " << rotatedZ.x << " y " << rotatedZ.y << " z " << rotatedZ.z  << endl;


    cout << "___________________________________________________" << endl;

    cout << "test direction (1, 0, -1), upward up" << endl;
    testDirection = vec3(1.0f, 0.0f, -1.0f);
    q = glm::quatLookAtLH(glm::normalize(testDirection), testup);
    cout << "quat w: " << q.w << " x " << q.x << " y " << q.y << " z " << q.z << endl;

    rotatedX = q * testright;
    rotatedY = q * testup;
    rotatedZ = q * testforward;
    cout << "rotated xaxis x " << rotatedX.x << " y " << rotatedX.y << " z " << rotatedX.z  << endl;
    cout << "rotated yaxis y " << rotatedY.x << " y " << rotatedY.y << " z " << rotatedY.z  << endl;
    cout << "rotated zaxis x " << rotatedZ.x << " y " << rotatedZ.y << " z " << rotatedZ.z  << endl;

    cout << "___________________________________________________" << endl;

    cout << "test direction (-1, -1, -1), upward up" << endl;
    testDirection = vec3(-1.0f, -1.0f, -1.0f);
    q = glm::quatLookAtLH(glm::normalize(testDirection), testup);
    cout << "quat w: " << q.w << " x " << q.x << " y " << q.y << " z " << q.z << endl;

    rotatedX = q * testright;
    rotatedY = q * testup;
    rotatedZ = q * testforward;
    cout << "rotated xaxis x " << rotatedX.x << " y " << rotatedX.y << " z " << rotatedX.z  << endl;
    cout << "rotated yaxis y " << rotatedY.x << " y " << rotatedY.y << " z " << rotatedY.z  << endl;
    cout << "rotated zaxis x " << rotatedZ.x << " y " << rotatedZ.y << " z " << rotatedZ.z  << endl;


    cout << "___________________________________________________" << endl;

    cout << "test direction (-1, -1, -1), upward down" << endl;
    testDirection = vec3(-1.0f, -1.0f, -1.0f);
    q = glm::quatLookAtLH(glm::normalize(testDirection), testdown);
    cout << "quat w: " << q.w << " x " << q.x << " y " << q.y << " z " << q.z << endl;

    rotatedX = q * testright;
    rotatedY = q * testup;
    rotatedZ = q * testforward;
    cout << "rotated xaxis x " << rotatedX.x << " y " << rotatedX.y << " z " << rotatedX.z  << endl;
    cout << "rotated yaxis y " << rotatedY.x << " y " << rotatedY.y << " z " << rotatedY.z  << endl;
    cout << "rotated zaxis x " << rotatedZ.x << " y " << rotatedZ.y << " z " << rotatedZ.z  << endl;
}

#endif  // _LIBCPP_CMATH
