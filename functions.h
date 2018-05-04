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
void draw_coins(void);

/* movement functions */
void move(void);
void jump(void);
void fall(void);
void move_platforms(void);

/* collision functions */
void collision_check(void);
void first_ground(void);
Box get_box(Platform);
bool get_collision(Box);
void coin_collision(void);

void game_over(void);
