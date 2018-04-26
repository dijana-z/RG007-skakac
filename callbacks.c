#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "callbacks.h"
#include "functions.h"

#define SPACE 32
#define ESC 27
#define MAX_KEYS 256
#define MAX_PLATFORMS 8

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
int coin_size = 8;

int level_no = 1;

int key_pressed[MAX_KEYS];

/* initial platform size and rotation */
int ground_size = 15;
int ground_rotation = 25;

/* jumping parameters */
float original_y = 0;
float translate_x = 0, translate_y = 0;
float move_y = 0.001, move_x = 6;

/* rotation parameter */
float angle_param = 10;

/*  window width and height sizes */
float window_width = 800, window_height = 800;

/* animation parameters */
int start_animation = 0;
int jump_up = 0;
int was_above = 0;

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
    /* set the initial position */
    player.y_position = -height/2 + ground_size + player.size/2;

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

    /* draw the initial platform */
    glPushMatrix();
    draw_platform();
    glPopMatrix();

    /* draw moving platforms and move them if needed */
    glPushMatrix();
    moving_platforms();
    move_platforms();
    glPopMatrix();

    /* draw and move the player if needed */
    glPushMatrix();
    glTranslatef(translate_x, translate_y, 0);
    glRotatef(angle_param, 0, 1, 0);
    jump();
    move();
    draw_player();
    glPopMatrix();

    glutSwapBuffers();
}
