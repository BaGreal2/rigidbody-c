#ifndef REIGIDBODYLIB_H
#define REIGIDBODYLIB_H

#include <stdlib.h>

typedef struct {
    float gravity_scale;
    float spring_scale;
    float damping_scale;
} RB_Config;

extern RB_Config rb_global_config;

typedef struct {
  float x;
  float y;
} RB_Vector2;

// Base bone structure.
// The main bone needs to have both joint1 and joint2 positions
// defined specifically. All other bones, if connected with rb_connect_bone
// can be defined with only join2 position, as joint1 will be set to
// joint2 position of the parent bone.
typedef struct {
  void *joint1;
  void *joint2;
  RB_Vector2 joint1_pos;
  RB_Vector2 joint2_pos;
  RB_Vector2 joint1_force;
  RB_Vector2 joint2_force;
  RB_Vector2 joint1_velocity;
  RB_Vector2 joint2_velocity;
  float joint1_mass;
  float joint2_mass;
  float length;
} RB_Bone;

void rb_init_config(const RB_Config *config);

float rb_calculate_distance(RB_Vector2 *v1, RB_Vector2 *v2);
void rb_calculate_object_resistance_force(RB_Bone *bone);
void rb_calculate_joint_resistance(RB_Bone *bone, float dt);

void rb_apply_gravity(RB_Bone *bone, float dt);

void rb_apply_force(RB_Bone *bone, float dt);
void rb_apply_velocity(RB_Bone *bone, float dt);
void rb_apply_joint_constraint(RB_Bone *parent, RB_Bone *child, float dt);

// Pre-update bones, calculating constraints for all bones.
// Needs to be called before rb_update_bones, as it expects
// pre-calculated resistance forces.
void rb_pre_update_bones(RB_Bone *bones, size_t bones_count);

// Can be used to update individual bone, applying all forces and constraints.
// You can as well do that by calling individual functions.
void rb_update_bone(RB_Bone *bone, float dt);
void rb_update_bones(RB_Bone *bones, size_t bones_count, float dt);

// Always connects parent joint2 to child and child joint1 to parent.
// joint1_pos of child bone will be always set to joint2_pos of the parent bone.
void rb_connect_bone(RB_Bone *parent, RB_Bone *child);

#endif // RIGIDBODYLIB_H
