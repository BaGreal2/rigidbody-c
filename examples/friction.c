#include "raylib.h"
#include "rigidbodylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define SCREEN_FPS 60
#define SCREEN_BACKGROUND (Color){22, 22, 22, 255}

#define GROUND_Y 400
#define FRICTION 1.5f

void apply_friction(RB_Bone *bone, float dt)
{
  if (bone->joint1_pos.y >= GROUND_Y) {
    float friction_force =
        FRICTION * (bone->joint1_mass * rb_global_config.gravity_scale);
    float friction_acc = friction_force / bone->joint1_mass;

    if (fabs(bone->joint1_velocity.x) > friction_acc * dt) {
      bone->joint1_velocity.x -=
          friction_acc * dt * (bone->joint1_velocity.x > 0 ? 1 : -1);
    } else {
      bone->joint1_velocity.x = 0;
    }
  }

  if (bone->joint2_pos.y >= GROUND_Y) {
    float friction_force =
        FRICTION * (bone->joint2_mass * rb_global_config.gravity_scale);
    float friction_acc = friction_force / bone->joint2_mass;
    if (fabs(bone->joint2_velocity.x) > friction_acc * dt) {
      bone->joint2_velocity.x -=
          friction_acc * dt * (bone->joint2_velocity.x > 0 ? 1 : -1);
    } else {
      bone->joint2_velocity.x = 0;
    }
  }
}

void detect_ground_collision(RB_Bone *bone)
{
  if (bone->joint1_pos.y > GROUND_Y) {
    bone->joint1_pos.y = GROUND_Y;
  }
  if (bone->joint2_pos.y > GROUND_Y) {
    bone->joint2_pos.y = GROUND_Y;
  }
}

void draw_bone(RB_Bone *bone)
{
  Vector2 joint1_pos = {bone->joint1_pos.x, bone->joint1_pos.y};
  Vector2 joint2_pos = {bone->joint2_pos.x, bone->joint2_pos.y};
  DrawLineEx(joint1_pos, joint2_pos, 2, WHITE);

  int circle_1_radius = 3 * bone->joint1_mass;
  int circle_2_radius = 3 * bone->joint2_mass;
  if (circle_1_radius) {
    DrawCircleV(joint1_pos, circle_1_radius, RED);
  } else {
    DrawCircleV(joint1_pos, 3, BLUE);
  }
  if (circle_2_radius) {
    DrawCircleV(joint2_pos, circle_2_radius, RED);
  } else {
    DrawCircleV(joint2_pos, 3, BLUE);
  }
}

int main()
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Friction Demonstation");
  SetTargetFPS(SCREEN_FPS);

  rb_init_config(0);

  Vector2 fixed_anchor = {SCREEN_WIDTH / 2.0f, 100};

  RB_Bone bone1 = {
      .joint1 = 0,
      .joint2 = 0,
      .joint1_pos = {50, 300},
      .joint2_pos = {100, 150},
      .joint1_mass = 1,
      .joint2_mass = 1,
      .joint1_velocity = {0, 0},
      .joint2_velocity = {0, 0},
      .length = rb_calculate_distance(&bone1.joint1_pos, &bone1.joint2_pos),
  };

  RB_Bone bone2 = {
      .joint1 = 0,
      .joint2 = 0,
      .joint1_pos = 0,
      .joint2_pos = {300, 300},
      .joint1_mass = 1,
      .joint2_mass = 1,
      .joint1_velocity = {0, 0},
      .joint2_velocity = {0, 0},
      .length = rb_calculate_distance(&bone2.joint1_pos, &bone2.joint2_pos),
  };

  RB_Bone bone3 = {
      .joint1 = 0,
      .joint2 = 0,
      .joint1_pos = 0,
      .joint2_pos = {400, 200},
      .joint1_mass = 1,
      .joint2_mass = 1,
      .joint1_velocity = {0, 0},
      .joint2_velocity = {0, 0},
      .length = rb_calculate_distance(&bone2.joint1_pos, &bone2.joint2_pos),
  };

  RB_Bone bones[] = {bone1, bone2, bone3};
  size_t bones_count = sizeof(bones) / sizeof(bones[0]);

  rb_connect_bone(&bones[0], &bones[1]);
  rb_connect_bone(&bones[1], &bones[2]);

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    rb_update_bones(bones, bones_count, dt);

    for (size_t i = 0; i < bones_count; ++i) {
      apply_friction(&bones[i], dt);
      detect_ground_collision(&bones[i]);
    }

    BeginDrawing();
    ClearBackground(SCREEN_BACKGROUND);

    DrawRectangle(0, GROUND_Y, SCREEN_WIDTH, SCREEN_HEIGHT - GROUND_Y, BROWN);

    for (size_t i = 0; i < bones_count; ++i) {
      draw_bone(&bones[i]);
    }
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
