#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <map>
#include <vector>
#include <cstdio>

#include "bone.h"

class Movement {
  private:
    /**
     * Vector represents sequence of maps, each map represents
     * angles of given bone in the given frame.
     */
    std::vector< std::map<unsigned long long, glm::vec3> > sequence;

    /**
     * Length moved already
     */
    std::vector< std::map<unsigned long long, glm::vec3> > moved;

    /**
     * Frame number
     */
    int position;

  public:
    /**
     * Create empty frame
     */
    Movement();

    /**
     * Move given bone by given angle (relative to previous frame)
     *
     * @param bone Bone to move
     * @param delta_angles Angles
     * @return this
     */
    Movement* move(Bone* bone, glm::vec3 delta_angles);

    /**
     * Set bone angles to given angles (absolute)
     *
     * @param bone Bone to move
     * @param angles Angles
     * @return this
     */
    Movement* set(Bone* bone, glm::vec3 angles);

    /**
     * Recursively set angles for every bone in the skeleton.
     *
     * @param root Bone to start
     * @return this
     */
    Movement* set(Bone* root);

    /**
     * Commit movements into single keyframe. Next position is set
     * automatically.
     *
     * @return this
     */
    Movement* keyframe();

    /**
     * Reset position
     *
     * @return this
     */
    Movement* start();

    /**
     * Mark next position
     *
     * @return true if success (there is next position)
     */
    bool next();

    /**
     * Calculate next frame - apply rotation to every bone
     * in current frame.
     *
     * @param fill Length of arc (in percent)
     * @param root Root bone for ids
     * @return true if success
     */
    bool frame(float fill, Bone* root);

    /**
     * @return Number of frames
     */
    int framesCount();
};

#endif
