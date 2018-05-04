#pragma once
#include <stdbool.h>

/* init functions */
void init_lighting(void);
void init_coordinates(void);
void init_platforms(void);
void update_player(void);

/* draw functions */
void draw_player(void);
void draw_platforms(void);
void draw_ground(void);
void draw_coin(float width, float height);

/* movement functions */
void move(void);
void jump(void);
void fall(void);
void move_platforms(void);
void collision_check(void);
void first_ground(void);

void game_over(void);

bool get_collision(Box);

Box get_box(Platform);
