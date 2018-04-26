#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <GL/glut.h>
#include "callbacks.h"
#include "functions.h"

#define MAX_PLATFORMS 8
#define MAX_COINS 4

extern int start_animation;
extern float window_width, window_height;
extern float cube_size;
extern float translate_x, move_x;
extern int key_pressed[];
extern int ground_size;
extern int ground_rotation;
extern int min_width;
extern int platform_size;
extern float moving_prob;
extern int level_no;
extern int platform_dist;
extern int coin_size;
extern float original_y;
extern float angle_param;
extern float move_y;
extern float translate_y;
extern int jump_up;
extern int was_above;

extern Platform platforms[];
extern Player player;
extern Coin *coins;


void init_lighting(void)
{
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat light_position[]  = { 1, 1, 1, 0 };
    GLfloat light_ambient[]   = { 0.0, 0.0, 0.0, 1 };
    GLfloat light_diffuse[]   = { 0.7, 0.7, 0.7, 1 };
    GLfloat light_specular[]  = { 0.9, 0.9, 0.9, 1 };
    GLfloat ambient_coeffs[]  = { 1.0, 0.1, 0.1, 1 };
    GLfloat diffuse_coeffs[]  = { 0.0, 0.0, 0.8, 1 };
    GLfloat specular_coeffs[] = { 1, 1, 1, 1 };
    GLfloat shininess = 40;

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void init_coordinates(void)
{
    /* initialize player state */
    player.size = 40;
    player.x_position = 0;
    player.ground = -1;

    /* set the moving platforms parameters */
    for(int i = 0; i < MAX_PLATFORMS; ++i) {
        platforms[i].scale_param = ((rand() % ((int)window_width/2 - min_width + 1)) + min_width)/platform_size;
        platforms[i].width = platforms[i].scale_param*platform_size;
        platforms[i].rotation = 30;
        platforms[i].has_coin = 0;
        platforms[i].pl_no = i+1;
        platforms[i].move = (rand()/(float)RAND_MAX < moving_prob) ? (rand() % 2 + 1) : 0;

        float   min_val = platforms[i].width/2 - window_width/2,
                initial_trans =  -window_height/2 + ground_size*2 + platform_size/2;
        platforms[i].x_position = (rand() % (int)(window_width - platforms[i].width + 1)) + min_val;
        platforms[i].y_position = initial_trans + platforms[i].pl_no*platform_dist;
    }

    for(int i = 0; i < level_no; ++i) {
        int rand_plat = rand() % 8;
        platforms[rand_plat].has_coin = 1;
// TODO: dinamicki novcici
        // coins[i]->size = coin_size;
        // coins[i]->x_position = platforms[i].x_position;
        // coins[i]->y_position = platforms[i].y_position + coin_size + 12;
    }
}

/* draw the player */
void draw_player(void)
{
    glColor3f(1, 0, 0);
    glTranslatef(player.x_position, player.y_position, 0);
    glutSolidCube(player.size);
}

/* draw the initial platform */
void draw_platform(void)
{
    glTranslatef(0, -window_height/2 + ground_size/2, 0);
    glRotatef(ground_rotation, 1, 0, 0);
    glScalef(window_width/ground_size, 1, 1);
    glColor3f(0, 1, 1);
    glutSolidCube(ground_size);
}

/* draw moving platforms */
void moving_platforms(void)
{
    for(int i = 0; i < MAX_PLATFORMS; ++i) {
        glPushMatrix();
            glColor3f(0.9, 0.5, 0.7);
            glTranslatef(platforms[i].x_position, platforms[i].y_position, -player.size/2);
            glRotatef(platforms[i].rotation, 1, 0, 0);
            glScalef(platforms[i].scale_param, 1, 1);
            glutSolidCube(platform_size);
        glPopMatrix();

        if(platforms[i].has_coin) {
            glPushMatrix();
                glColor3f(1, 1, 0);
                glTranslatef(platforms[i].x_position, platforms[i].y_position + coin_size + 15, -player.size/2);
                glutSolidSphere(coin_size, 15, 15);
            glPopMatrix();
        }
    }
}

/* x-axis movement function for player */
void move(void)
{
    if(key_pressed['a']) {
        if(translate_x - move_x - 5 >= -window_width/2 + player.size/2) {
            translate_x -= move_x;
        }
    } else if(key_pressed['d']) {
        if(translate_x + move_x + 5 <= window_width/2 - player.size/2) {
            translate_x += move_x;
        }
    }
}

/* x-axis movement function for moving platforms */
void move_platforms(void)
{
    if(start_animation) {
        for(int i = 0; i < MAX_PLATFORMS; ++i) {
            if(fabs(platforms[i].x_position + platforms[i].move) <
               window_width/2 - platforms[i].width/2) {
                platforms[i].x_position += platforms[i].move;
            } else {
                 platforms[i].move *= -1;
            }
        }
    }
}

/* jumping */
void jump(void)
{
// TODO: add gravity and fix parameters
    if(jump_up) {
        angle_param += 5 % 360;
        translate_y = 130*sin(move_y);

        for(int i = 0; i < MAX_PLATFORMS; ++i) {
            if(get_collision(get_box(platforms[i]))) {
                player.ground = i;
                move_y = 0.001;
                was_above = 0;
                jump_up = 0;
            }
        }

        move_y += 0.05;

        if(move_y >= 3.14) {
            move_y = 0;
        }

        if(translate_y <= original_y) {
            move_y = 0.001;
            jump_up = 0;
            was_above = 0;
        }
    }
}

Box get_box(Platform platform)
{
    Box box = {
    .y_top = platform.y_position + platform_size/2,
    .y_bottom = platform.y_position - platform_size/2,
    .x_left = platform.x_position - platform.width/2,
    .x_right = platform.x_position + platform.width/2
    };

    return box;
}

bool get_collision(Box box)
{
    float   player_head = player.y_position + translate_y + player.size/2,
            player_bottom = player.y_position + translate_y - player.size/2,
            player_left = player.x_position + translate_x - player.size/2,
            player_right = player.x_position + translate_x +  player.size/2;

    if(player_bottom > box.y_top) {
        was_above = 1;
    }

    if(player_head > box.y_top && player_bottom <= box.y_top && was_above &&
        player_left >= box.x_left && player_right <= box.x_right) {
        return true;
    }

    return false;
}


void gravity(void)
{

}
