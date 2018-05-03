#pragma once
#include <stdbool.h>

/* init functions */
void init_lighting(void);
void init_coordinates(void);

/* draw functions */
void draw_player(void);
void moving_platforms(void);

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
