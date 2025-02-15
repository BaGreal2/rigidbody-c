// #include "raylib.h"
// #include <math.h>
// #include <stdlib.h>
//
// #define SCREEN_WIDTH 800
// #define SCREEN_HEIGHT 450
// #define SCREEN_FPS 60
// #define SCREEN_BACKGROUND (Color){22, 22, 22, 255}
//
// #define GROUND_Y 400
//
// #define G 30
// #define K 10
//
// typedef struct {
//   void *joint1;
//   void *joint2;
//   Vector2 joint1_pos;
//   Vector2 joint2_pos;
//   float mass;
//   float length;
//   Vector2 velocity;
// } Bone;
//
// float calculate_distance(Vector2 *v1, Vector2 *v2)
// {
//   float dx = v2->x - v1->x;
//   float dy = v2->y - v1->y;
//   return sqrtf(dx * dx + dy * dy);
// }
//
// void apply_gravity(Bone *bone, float dt) { bone->velocity.y += G * dt; }
//
// void apply_velocity(Bone *bone, float dt)
// {
//   bone->joint1_pos.x += bone->velocity.x * dt;
//   bone->joint1_pos.y += bone->velocity.y * dt;
//   bone->joint2_pos.x += bone->velocity.x * dt;
//   bone->joint2_pos.y += bone->velocity.y * dt;
// }
//
// void apply_object_resistence_force(Bone *bone, float dt)
// {
//   float dx = bone->joint2_pos.x - bone->joint1_pos.x;
//   float dy = bone->joint2_pos.y - bone->joint1_pos.y;
//   float distance = sqrtf(dx * dx + dy * dy);
//   float force = K * (distance - bone->length);
//   float angle = atan2f(dy, dx);
//
//   bone->joint1_pos.x += force * cosf(angle) * dt;
//   bone->joint1_pos.y += force * sinf(angle) * dt;
//   bone->joint2_pos.x -= force * cosf(angle) * dt;
//   bone->joint2_pos.y -= force * sinf(angle) * dt;
// }
//
// void detect_ground_collision(Bone *bone)
// {
//   if (bone->joint1_pos.y > GROUND_Y) {
//     bone->joint1_pos.y = GROUND_Y;
//   }
//   if (bone->joint2_pos.y > GROUND_Y) {
//     bone->joint2_pos.y = GROUND_Y;
//   }
// }
//
// void draw_bone(Bone *bone)
// {
//   DrawLineEx(bone->joint1_pos, bone->joint2_pos, 2, WHITE);
// }
//
// int main()
// {
//   InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rigidbody Physics");
//   SetTargetFPS(SCREEN_FPS);
//
//   Bone bone1 = {
//       .joint1 = 0,
//       .joint2 = 0,
//       .joint1_pos = {150, 100},
//       .joint2_pos = {100, 200},
//       .mass = 1,
//       .length = calculate_distance(&bone1.joint1_pos, &bone1.joint2_pos),
//       .velocity = {0, 0},
//   };
//
//   Bone bone2 = {
//       .joint1 = &bone1,
//       .joint2 = 0,
//       .joint1_pos = {100, 200},
//       .joint2_pos = {80, 300},
//       .mass = 1,
//       .length = calculate_distance(&bone2.joint1_pos, &bone2.joint2_pos),
//       .velocity = {0, 0},
//   };
//
//   Bone bones[] = {bone1, bone2};
//   size_t bones_count = sizeof(bones) / sizeof(bones[0]);
//
//   while (!WindowShouldClose()) {
//     BeginDrawing();
//
//     float dt = GetFrameTime();
//
//     for (size_t i = 0; i < bones_count; ++i) {
//       apply_object_resistence_force(&bones[i], dt);
//       apply_gravity(&bones[i], dt);
//       apply_velocity(&bones[i], dt);
//       detect_ground_collision(&bones[i]);
//     }
//
//     DrawRectangle(0, GROUND_Y, SCREEN_WIDTH, SCREEN_HEIGHT - GROUND_Y, BROWN);
//     for (size_t i = 0; i < bones_count; ++i) {
//       draw_bone(&bones[i]);
//     }
//
//     ClearBackground(SCREEN_BACKGROUND);
//     EndDrawing();
//   }
//
//   CloseWindow();
//
//   return 0;
// }
