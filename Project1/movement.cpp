#include "movement.h"

#include <algorithm>

glm::vec3 sgn(glm::vec3 a, glm::vec3 b) {
  return glm::vec3(
    a.x >= b.x ? -1 : 1,
    a.y >= b.y ? -1 : 1,
    a.z >= b.z ? -1 : 1
  );
}

Movement::Movement() {
  position = 0;
  sequence.resize(1);
}

Movement* Movement::move(Bone* bone, glm::vec3 delta_angles) {
  return set(bone, sequence[position][bone->id()]+delta_angles);
}

Movement* Movement::set(Bone* bone, glm::vec3 angles) {
  sequence[position][bone->id()] = angles;

  return this;
}

Movement* Movement::keyframe() {
  position++;
  sequence.resize(position+1);

  sequence[position] = sequence[position-1];

  return this;
}

Movement* Movement::set(Bone* root) {
  sequence[position][root->id()] = glm::vec3(root->rotation);

  for (std::vector<Bone*>::iterator it = root->bones.begin(); it != root->bones.end(); ++it) {
    set(*it);
  }


  return this;
}

Movement* Movement::start() {
  position = 0;
  moved.clear();
  moved.resize(sequence.size());

  for (int i = 0; i < sequence.size(); i++) {
    for (std::map<unsigned long long,glm::vec3>::iterator kv = sequence[i].begin(); kv != sequence[i].end(); ++kv) {
      moved[i][kv->first] = glm::vec3(0.0f);
    }
  }

  return this;
}

bool Movement::frame(float fill, Bone* root) {
  if (position >= framesCount()) {
    return false;
  }

  int c = 0;
  int d = 0;
  Bone* current;

  for (std::map<unsigned long long,glm::vec3>::iterator kv = sequence[position].begin(); kv != sequence[position].end(); ++kv) {
    glm::vec3 dv = glm::abs(sequence[position-1][kv->first] - kv->second);
/*
    if (dv.x > 180)
      dv.x = 360 - dv.x;

    if (dv.y > 180)
      dv.y = 360 - dv.y;

    if (dv.z > 180)
      dv.z = 360 - dv.z;
*/
    glm::vec3 s = sgn(sequence[position-1][kv->first], kv->second);

    //d += fabs(dv.x) < 0.01f;
    //d += fabs(dv.y) < 0.01f;
    //d += fabs(dv.z) < 0.01f;

    dv.x *= s.x * fill;
    dv.y *= s.y * fill;
    dv.z *= s.z * fill;

    moved[position][kv->first] += glm::abs(dv);
    current = root->bone(kv->first);

    d+=3;

    if (glm::abs(moved[position][kv->first]).x >= glm::abs(sequence[position-1][kv->first] - kv->second).x) {
      c++;
    } else {
      current->rotate(dv.x, 0, 0);
    }

    if (glm::abs(moved[position][kv->first]).y >= glm::abs(sequence[position-1][kv->first] - kv->second).y) {
        c++;
    } else {
      current->rotate(0, dv.y, 0);
    }

    if (glm::abs(moved[position][kv->first]).z >= glm::abs(sequence[position-1][kv->first] - kv->second).z) {
        c++;
    } else {
      current->rotate(0, 0, dv.z);
    }
  }

  printf("c=%d, d=%d\n", c,d);
  return c < d;
}

bool Movement::next() {
  if (position >= framesCount())
    return false;

  position++;

  return true;
}

int Movement::framesCount() {
  return sequence.size();
}


