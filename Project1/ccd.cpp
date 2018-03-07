#/*********************************************************************/
//   ccd.cpp -- Implementation of Inverse Kinematics using CCD	   *
//		 Author:  Adwitiya Chakraborty                                 *
//                                                                     *
//      Purpose: CS7GV5 Real Time Animation - Assignment 3 	           *
//                                                                     *
// GitHub Repo : https://github.com/adwitiya/InverseKinematics		   *
//		 Email : chakraad@tcd.ie									   *
//  Build Date : 06.03.2018											   *
#/*********************************************************************/
#include "ccd.h"
#include "helpers.h"
#include <cstdio>

void ccd::findNewAngles(Bone *endEffector, vec3 target, int iterations) {
  bool found = false;
  while(!found && iterations--) {
    Bone * currentBone = endEffector;
    while(currentBone->parent != NULL) {
      vec4 endPosition = currentBone->getEndPosition();
      vec4 startPosition = currentBone->parent->getEndPosition();
      vec3 toTarget = normalize(vec3(target.x-startPosition.x, target.y-startPosition.y, target.z-startPosition.z));
      vec3 toEnd = normalize(vec3(endPosition.x-startPosition.x, endPosition.y-startPosition.y, endPosition.z-startPosition.z));
      float cosine = dot(toEnd, toTarget);
      if(cosine < 0.99) {
        vec3 crossResult = cross(toEnd, toTarget);
        float angle = glm::angle(toTarget, toEnd);
        quat rotation = angleAxis(angle, crossResult);
        rotation = normalize(rotation);
        glm::vec3 euler = glm::eulerAngles(rotation) ;
        currentBone->rotateMax(euler.x, euler.y, euler.z);
      }

      vec3 temp = vec3(endEffector->getEndPosition());
      temp.x -= target.x; 
	  temp.y -= target.y; 
	  temp.z -= target.z;
      if(dot(temp, temp) < 0.001) {
        found = true;
        printf("target reached\n");
      }
      currentBone = currentBone->parent;
    }
  }
}

