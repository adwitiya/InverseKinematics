#include "bone.h"
#include "helpers.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include <cstdio>

Bone::Bone(float l) {
  length = l;
  M = glm::mat4(1.0f);
  parent = NULL;
  rotation = glm::vec3(0.0f);
  coordinates = glm::vec3(0.0f);
  constraint[0] = glm::vec3(-360.0f);
  constraint[1] = glm::vec3(360.0f);
};

Bone::Bone(const Bone& b) {
  length = b.length;
  M = glm::mat4(b.M);
  parent = NULL;
  rotation = glm::vec3(b.rotation);
  coordinates = glm::vec3(b.coordinates);
  constraint[0] = glm::vec3(b.constraint[0]);
  constraint[1] = glm::vec3(b.constraint[1]);

  for (std::vector< Bone* >::const_iterator it = b.bones.begin(); it != b.bones.end(); ++it) {
    add(new Bone(**it));
  }
}

Bone* Bone::add(Bone *b) {
	b->parent = this;
	bones.push_back(b);

  return b;
}

void Bone::remove(Bone *b) {
	b->parent = NULL;
	bones.erase(std::remove(bones.begin(), bones.end(), b), bones.end());

	delete b;
}

glm::vec4 Bone::getEndPosition() {
	return glm::translate(getTransform(), glm::vec3(0.0f, 0.0f, length)) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

glm::mat4 Bone::getTransform() {
	glm::mat4 P = glm::mat4();

	if (parent != NULL) {
		P = glm::translate(P, glm::vec3(0.0f, 0.0f, parent->length));

		P = glm::rotate(P, rotation.x, glm::vec3(P*glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
		P = glm::rotate(P, rotation.y, glm::vec3(P*glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
		P = glm::rotate(P, rotation.z, glm::vec3(P*glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));

		P = parent->getTransform() * P;
	} else {
		P = glm::rotate(P, 90.0f, glm::vec3(-1.0f, 0.0f, 0.0f));
	}

	return P;
}

Bone* Bone::constraints(float nx, float mx, float ny, float my, float nz, float mz) {
  constraint[0] = glm::vec3(nx, ny, nz);
  constraint[1] = glm::vec3(mx, my, mz);

  return this;
}

void Bone::checkConstraints(float x, float y, float z) {
  if (x < constraint[0].x || x > constraint[1].x ||
      y < constraint[0].y || y > constraint[1].y ||
      z < constraint[0].z || z > constraint[1].z) {
    printf("%f, %f, %f\n(%f,%f), (%f,%f), (%f,%f)\n",x,y,z,constraint[0].x,constraint[1].x,constraint[0].y,constraint[1].y,constraint[0].z,constraint[1].z);
    throw new ConstraintException();
  }
}

Bone* Bone::setRotate(float x, float y, float z) {
  float lx, ly, lz;

  checkConstraints(lx = fmod(x, 360.0f), ly = fmod(y, 360.f), lz = fmod(z, 360.0f));


  rotation.x = lx;
  rotation.y = ly;
  rotation.z = lz;

  return this;
}

Bone* Bone::rotateMax(float dx, float dy, float dz) {
  if(dx+rotation.x > 180) dx -= 360;
  if(dy+rotation.y > 180) dy -= 360;
  if(dz+rotation.z > 180) dz -= 360;
  if(dx+rotation.x < -180) dx += 360;
  if(dy+rotation.y < -180) dy += 360;
  if(dz+rotation.z < -180) dz += 360;
  if(dx+rotation.x > constraint[1].x) dx = constraint[1].x-rotation.x - 0.1f;
  if(dx+rotation.x < constraint[0].x) dx = constraint[0].x-rotation.x+0.1f;
  if(dy+rotation.y > constraint[1].y) dy = constraint[1].y-rotation.y-0.1f;
  if(dy+rotation.y < constraint[0].y) dy = constraint[0].y-rotation.y+0.1f;
  if(dz+rotation.z > constraint[1].z) dz = constraint[1].z-rotation.z-0.1f;
  if(dz+rotation.z < constraint[0].z) dz = constraint[0].z-rotation.z+0.1f;
  rotate(dx,dy,dz);
  return this;
}

Bone* Bone::rotate(float dx, float dy, float dz) {
  return setRotate(rotation.x+dx, rotation.y+dy, rotation.z+dz);

  return this;
}

void Bone::detach() {
	parent->bones.erase(std::remove(bones.begin(), bones.end(), this), bones.end());
	parent = NULL;
}

Bone* Bone::bone(unsigned long long id) {
  if (id < 10) {
    return bones[id-1];
  } else {
    return bones[id % 10 - 1]->bone(id/10);
  }
}

unsigned long long Bone::id() {
  if (parent == NULL)
    return 0;
  else
    return parent->id()*10 + (std::find(parent->bones.begin(), parent->bones.end(), this) - parent->bones.begin()) + 1;
}

Bone::~Bone() {
	for (std::vector<Bone*>::iterator it = bones.begin(); it != bones.end(); it++) {
		delete *it;
	}
}

