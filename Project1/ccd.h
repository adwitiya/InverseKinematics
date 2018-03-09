#ifndef CCD_H
#define CCD_H

#include "bone.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtx/vector_angle.hpp"
using namespace glm;

namespace ccd {
  void findNewAngles(Bone *endEffector, vec3 target, int iterations=1000);
  //quat rotationBetweenVectors(vec3 start, vec3 dest);
}

#endif
