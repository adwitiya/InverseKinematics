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
        float a = glm::angle(toTarget, toEnd);
        quat rot = angleAxis(a, crossResult);
        rot = normalize(rot);
        glm::vec3 euler = glm::eulerAngles(rot) ;
        currentBone->rotateMax(euler.x, euler.y, euler.z);
      }

      vec3 tmp = vec3(endEffector->getEndPosition());
      tmp.x -= target.x; tmp.y -= target.y; tmp.z -= target.z;
      if(dot(tmp, tmp) < 0.001) {
        found = true;
        printf("found\n");
      }
      currentBone = currentBone->parent;
    }
  }
}

