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

void level_upgrade(void);

/* collision functions */
void collision_check(void);
void first_ground(void);
Box get_box(Platform);
Box coin_box(Coin);
bool get_collision(Box);
bool coin_coll_check(Box);
void coin_collision(void);

/* world moving */
void start_moving(void);

/* displaying text */
void set_the_text(void);
void text_display(char *str, float x, float y, float z);
void highscores(void);

/* textures */
void initialize_texture(void);
void platform_texture(int i);
void ground_texture(void);
void background_texture(void);
Point* get_platform_coords(int i);
