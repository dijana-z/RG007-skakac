#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "callbacks.h"
#include "functions.h"

#define SPACE 32
#define ESC 27
#define MAX_KEYS 256
#define MAX_PLATFORMS 9

#define TIMER_INT 10
#define TIMER0_ID 0
#define TIMER1_ID 1

/* camera position parameters */
float cam_eye_x = 0, cam_eye_y = 0, cam_eye_z = 10;

/* init moving platform parameters */
int min_width = 200;
int platform_size = 15;
float moving_prob = 0;
int platform_dist = 85;
int coin_size = 12;
int platform_rotation = 25;

float coin_param = 15, delta_coin = 0.2;

int level_no = 1;

int key_pressed[MAX_KEYS];

/* jumping parameters */
float translate_x = 0, translate_y = 0;
float move_y = 0.001, move_x = 6, helping_par = 5;
int ground = 0;

/* rotation parameter */
float angle_param = 10;

/*  window width and height sizes */
float window_width = 800, window_height = 800;

/* animation parameters */
int start_animation = 0;
int jump_up = 0, falling = 0;
int was_above = 0;
float gravity = 1.1;
int start = 1;
float x = 3.5, delta_jump = 0.08, delta_angle = 5 % 360;

Player player;
Platform platforms[MAX_PLATFORMS];
// Coin *coins = NULL;

void on_keyboard(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    switch (key) {
    /* exit the game */
        case ESC:
            exit(EXIT_SUCCESS);
            break;
        /* start or stop the game */
        case SPACE:
            if(!start_animation) {
                start_animation = 1;
                glutTimerFunc(TIMER_INT, on_timer, TIMER0_ID);
            } else {
                start_animation = 0;
            }
            break;
        /* jump up */
        case 'W':
        case 'w':
            if(start_animation) {
                if(!jump_up){
                    jump_up = 1;
                }
            }
            break;
        /* go left */
        case 'a':
        case 'A':
            if(start_animation) {
                key_pressed['a'] = 1;
                glutTimerFunc(TIMER_INT, on_timer, TIMER0_ID);
            }
            break;
        /* go right */
        case 'd':
        case 'D':
            if(start_animation) {
                key_pressed['d'] = 1;
                glutTimerFunc(TIMER_INT, on_timer, TIMER0_ID);
            }
            break;
    }
}

void on_key_release(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    switch (key) {
        case 'a':
        case 'A':
            key_pressed['a'] = 0;
            break;
        case 'd':
        case 'D':
            key_pressed['d'] = 0;
                break;
    }
}

void on_reshape(int width, int height)
{
    /* set the window width and height parameters */
    window_height = height;
    window_width = width;

    init_platforms();
    update_player();

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-width/2, width/2, -height/2, height/2, -100, 100);
    glMatrixMode(GL_MODELVIEW);
}

void on_timer(int value)
{
    if(value != TIMER0_ID) {
        return;
    }

    glutPostRedisplay();

    if(start_animation) {
        start_animation = 1;
        glutTimerFunc(TIMER_INT, on_timer, TIMER0_ID);
    }
}

void on_display(void)
{
    /* clear the window */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* set the camera eye */
    glLoadIdentity();
    gluLookAt(
        cam_eye_x, cam_eye_y, cam_eye_z,
        0, 0, 0,
        0, 1, 0
    );

    /* draw moving platforms and move them if needed */
    glPushMatrix();
    draw_platforms();
    move_platforms();
    glPopMatrix();

    /* draw and move the player if needed */
    glPushMatrix();
    first_ground();
    collision_check();
    fall();
    jump();
    move();
    draw_player();
    glPopMatrix();

    glutSwapBuffers();
}
