#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define SCREEN_FPS 60
#define SCREEN_BACKGROUND (Color){22, 22, 22, 255}

#define GROUND_Y 400

#define G 30
#define K 200
#define D 10
#define FRICTION 5.0f

typedef struct {
  void *joint1;
  void *joint2;
  Vector2 joint1_pos;
  Vector2 joint2_pos;
  Vector2 joint1_force;
  Vector2 joint2_force;
  Vector2 joint1_velocity;
  Vector2 joint2_velocity;
  float joint1_mass;
  float joint2_mass;
  float length;
} Bone;

float calculate_distance(Vector2 *v1, Vector2 *v2)
{
  float dx = v2->x - v1->x;
  float dy = v2->y - v1->y;
  return sqrtf(dx * dx + dy * dy);
}

void apply_gravity(Bone *bone, float dt)
{
  bone->joint1_velocity.y += G * dt;
  bone->joint2_velocity.y += G * dt;
}

void apply_friction(Bone *bone, float dt)
{
  if (bone->joint1_pos.y >= GROUND_Y) {
    float friction_force = FRICTION * (bone->joint1_mass * G);
    float friction_acc = friction_force / bone->joint1_mass;

    if (fabs(bone->joint1_velocity.x) > friction_acc * dt) {
      bone->joint1_velocity.x -=
          friction_acc * dt * (bone->joint1_velocity.x > 0 ? 1 : -1);
    } else {
      bone->joint1_velocity.x = 0;
    }
  }

  if (bone->joint2_pos.y >= GROUND_Y) {
    float frictionForce = FRICTION * (bone->joint2_mass * G);
    float frictionAccel = frictionForce / bone->joint2_mass;
    if (fabs(bone->joint2_velocity.x) > frictionAccel * dt) {
      bone->joint2_velocity.x -=
          frictionAccel * dt * (bone->joint2_velocity.x > 0 ? 1 : -1);
    } else {
      bone->joint2_velocity.x = 0;
    }
  }
}

void apply_force(Bone *bone, float dt)
{
  bone->joint1_velocity.x += bone->joint1_force.x / bone->joint1_mass * dt;
  bone->joint1_velocity.y += bone->joint1_force.y / bone->joint1_mass * dt;
  bone->joint2_velocity.x += bone->joint2_force.x / bone->joint2_mass * dt;
  bone->joint2_velocity.y += bone->joint2_force.y / bone->joint2_mass * dt;
}

void apply_velocity(Bone *bone, float dt)
{
  bone->joint1_pos.x += bone->joint1_velocity.x * dt;
  bone->joint1_pos.y += bone->joint1_velocity.y * dt;
  bone->joint2_pos.x += bone->joint2_velocity.x * dt;
  bone->joint2_pos.y += bone->joint2_velocity.y * dt;
}

void calculate_object_resistance_force(Bone *bone)
{
  float dx = bone->joint2_pos.x - bone->joint1_pos.x;
  float dy = bone->joint2_pos.y - bone->joint1_pos.y;
  float distance = sqrtf(dx * dx + dy * dy);

  float angle = atan2f(dy, dx);

  float rel_vel =
      ((bone->joint2_velocity.x - bone->joint1_velocity.x) * cosf(angle) +
       (bone->joint2_velocity.y - bone->joint1_velocity.y) * sinf(angle));

  float spring_force = K * (distance - bone->length);
  float damping_force = D * rel_vel;

  float force = spring_force + damping_force;

  bone->joint1_force.x = force * cosf(angle);
  bone->joint1_force.y = force * sinf(angle);
  bone->joint2_force.x = -force * cosf(angle);
  bone->joint2_force.y = -force * sinf(angle);
}

void apply_joint_constraint(Bone *parent, Bone *child)
{
  Vector2 error = {child->joint1_pos.x - parent->joint2_pos.x,
                   child->joint1_pos.y - parent->joint2_pos.y};

  Vector2 rel_vel = {child->joint1_velocity.x - parent->joint2_velocity.x,
                     child->joint1_velocity.y - parent->joint2_velocity.y};

  Vector2 corrective_force = {K * error.x + D * rel_vel.x,
                              K * error.y + D * rel_vel.y};

  parent->joint2_force.x += corrective_force.x * 0.5f;
  parent->joint2_force.y += corrective_force.y * 0.5f;
  child->joint1_force.x -= corrective_force.x * 0.5f;
  child->joint1_force.y -= corrective_force.y * 0.5f;
}

void calculate_joint_resistance(Bone *bone)
{
  if (bone->joint1) {
    Bone *parent = (Bone *)bone->joint1;
    apply_joint_constraint(parent, bone);
  }
  if (bone->joint2) {
    Bone *child = (Bone *)bone->joint2;
    apply_joint_constraint(bone, child);
  }
}

void detect_ground_collision(Bone *bone)
{
  if (bone->joint1_pos.y > GROUND_Y) {
    bone->joint1_pos.y = GROUND_Y;
  }
  if (bone->joint2_pos.y > GROUND_Y) {
    bone->joint2_pos.y = GROUND_Y;
  }
}

void draw_bone(Bone *bone)
{
  DrawLineEx(bone->joint1_pos, bone->joint2_pos, 2, WHITE);
}

int main()
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rigidbody Physics");
  SetTargetFPS(SCREEN_FPS);

  Bone bone1 = {
      .joint1 = 0,
      .joint2 = 0,
      .joint1_pos = {40, 300},
      .joint2_pos = {100, 200},
      .joint1_mass = 1,
      .joint2_mass = 1,
      .joint1_velocity = {0, 0},
      .joint2_velocity = {0, 0},
      .length = calculate_distance(&bone1.joint1_pos, &bone1.joint2_pos),
  };

  Bone bone2 = {
      .joint1 = 0,
      .joint2 = 0,
      .joint1_pos = {100, 200},
      .joint2_pos = {160, 300},
      .joint1_mass = 1,
      .joint2_mass = 1,
      .joint1_velocity = {0, 0},
      .joint2_velocity = {0, 0},
      .length = calculate_distance(&bone2.joint1_pos, &bone2.joint2_pos),
  };

  Bone bone3 = {
      .joint1 = 0,
      .joint2 = 0,
      .joint1_pos = {160, 300},
      .joint2_pos = {220, 200},
      .joint1_mass = 1,
      .joint2_mass = 1,
      .joint1_velocity = {0, 0},
      .joint2_velocity = {0, 0},
      .length = calculate_distance(&bone3.joint1_pos, &bone3.joint2_pos),
  };

  Bone bones[] = {bone1, bone2, bone3};
  bones[0].joint2 = &bones[1];
  bones[1].joint1 = &bones[0];
  bones[1].joint2 = &bones[2];
  bones[2].joint1 = &bones[1];
  size_t bones_count = sizeof(bones) / sizeof(bones[0]);

  while (!WindowShouldClose()) {
    BeginDrawing();

    float dt = GetFrameTime();

    for (size_t i = 0; i < bones_count; ++i) {
      calculate_object_resistance_force(&bones[i]);
    }

    for (size_t i = 0; i < bones_count; ++i) {
      calculate_joint_resistance(&bones[i]);
      apply_gravity(&bones[i], dt);
      apply_friction(&bones[i], dt);
      apply_force(&bones[i], dt);
      apply_velocity(&bones[i], dt);
      detect_ground_collision(&bones[i]);
    }

    DrawRectangle(0, GROUND_Y, SCREEN_WIDTH, SCREEN_HEIGHT - GROUND_Y, BROWN);
    for (size_t i = 0; i < bones_count; ++i) {
      draw_bone(&bones[i]);
    }

    ClearBackground(SCREEN_BACKGROUND);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
