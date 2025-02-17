#include "rigidbodylib.h"
#include <math.h>
#include <stdio.h>

RB_Config rb_global_config;

void rb_init_config(const RB_Config *config)
{
  if (config) {
    rb_global_config = *config;
  } else {
    rb_global_config.gravity_scale = 200;
    rb_global_config.spring_scale = 600;
    rb_global_config.damping_scale = 50;
  }
}

float rb_calculate_distance(RB_Vector2 *v1, RB_Vector2 *v2)
{
  float dx = v2->x - v1->x;
  float dy = v2->y - v1->y;
  return sqrtf(dx * dx + dy * dy);
};

void rb_calculate_object_resistance_force(RB_Bone *bone)
{
  float dx = bone->joint2_pos.x - bone->joint1_pos.x;
  float dy = bone->joint2_pos.y - bone->joint1_pos.y;
  float distance = sqrtf(dx * dx + dy * dy);

  float angle = atan2f(dy, dx);

  float rel_vel =
      ((bone->joint2_velocity.x - bone->joint1_velocity.x) * cosf(angle) +
       (bone->joint2_velocity.y - bone->joint1_velocity.y) * sinf(angle));

  float spring_force =
      rb_global_config.spring_scale * (distance - bone->length);
  float damping_force = rb_global_config.damping_scale * rel_vel;

  float force = spring_force + damping_force;

  bone->joint1_force.x = force * cosf(angle);
  bone->joint1_force.y = force * sinf(angle);
  bone->joint2_force.x = -force * cosf(angle);
  bone->joint2_force.y = -force * sinf(angle);
}

void rb_calculate_joint_resistance(RB_Bone *bone, float dt)
{
  if (bone->joint1) {
    RB_Bone *parent = (RB_Bone *)bone->joint1;
    rb_apply_joint_constraint(parent, bone, dt);
  }
  if (bone->joint2) {
    RB_Bone *child = (RB_Bone *)bone->joint2;
    rb_apply_joint_constraint(bone, child, dt);
  }
}

void rb_apply_gravity(RB_Bone *bone, float dt)
{
  if (bone->joint1_mass != 0) {
    bone->joint1_velocity.y += rb_global_config.gravity_scale * dt;
  }
  if (bone->joint2_mass != 0) {
    bone->joint2_velocity.y += rb_global_config.gravity_scale * dt;
  }
}

void rb_apply_force(RB_Bone *bone, float dt)
{
  if (bone->joint1_mass != 0) {
    bone->joint1_velocity.x += bone->joint1_force.x / bone->joint1_mass * dt;
    bone->joint1_velocity.y += bone->joint1_force.y / bone->joint1_mass * dt;
  }
  if (bone->joint2_mass != 0) {
    bone->joint2_velocity.x += bone->joint2_force.x / bone->joint2_mass * dt;
    bone->joint2_velocity.y += bone->joint2_force.y / bone->joint2_mass * dt;
  }
}

void rb_apply_velocity(RB_Bone *bone, float dt)
{
  bone->joint1_pos.x += bone->joint1_velocity.x * dt;
  bone->joint1_pos.y += bone->joint1_velocity.y * dt;
  bone->joint2_pos.x += bone->joint2_velocity.x * dt;
  bone->joint2_pos.y += bone->joint2_velocity.y * dt;
}

void rb_apply_joint_constraint(RB_Bone *parent, RB_Bone *child, float dt)
{
    RB_Vector2 error = {child->joint1_pos.x - parent->joint2_pos.x,
                        child->joint1_pos.y - parent->joint2_pos.y};

    float distance = sqrtf(error.x * error.x + error.y * error.y);
    float correction_factor = (distance - child->length) * 0.5f;

    if (distance > 0) {
        error.x /= distance;
        error.y /= distance;
    }

    // Apply position correction
    parent->joint2_pos.x -= correction_factor * error.x * dt;
    parent->joint2_pos.y -= correction_factor * error.y * dt;
    child->joint1_pos.x += correction_factor * error.x * dt;
    child->joint1_pos.y += correction_factor * error.y * dt;

    // Apply velocity correction
    RB_Vector2 rel_vel = {child->joint1_velocity.x - parent->joint2_velocity.x,
                          child->joint1_velocity.y - parent->joint2_velocity.y};

    float corrective_vel_x = rb_global_config.damping_scale * rel_vel.x;
    float corrective_vel_y = rb_global_config.damping_scale * rel_vel.y;

    parent->joint2_velocity.x += corrective_vel_x * 0.5f * dt;
    parent->joint2_velocity.y += corrective_vel_y * 0.5f * dt;
    child->joint1_velocity.x -= corrective_vel_x * 0.5f * dt;
    child->joint1_velocity.y -= corrective_vel_y * 0.5f * dt;
}

void rb_pre_update_bones(RB_Bone *bones, size_t bones_count)
{
  for (size_t i = 0; i < bones_count; ++i) {
    rb_calculate_object_resistance_force(&bones[i]);
  }
}

void rb_update_bone(RB_Bone *bone, float dt)
{
  rb_calculate_object_resistance_force(bone);
  rb_calculate_joint_resistance(bone, dt);
  rb_apply_gravity(bone, dt);
  rb_apply_force(bone, dt);
  rb_apply_velocity(bone, dt);
}

void rb_update_bones(RB_Bone *bones, size_t bones_count, float dt)
{
  rb_pre_update_bones(bones, bones_count);

  for (size_t i = 0; i < bones_count; ++i) {
    rb_update_bone(&bones[i], dt);
  }
}

void rb_connect_bone(RB_Bone *parent, RB_Bone *child)
{
  child->joint1 = parent;
  child->joint1_pos = parent->joint2_pos;
  child->length = rb_calculate_distance(&child->joint1_pos, &child->joint2_pos);
  parent->joint2 = child;
}
