#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include "callbacks.h"
#include "functions.h"

#define SPACE 32
#define ESC 27
#define MAX_KEYS 256

#define TIMER_INT 10
#define TIMER0_ID 0
#define TIMER1_ID 1

/* camera position parameters */
float cam_eye_x = 0, cam_eye_y = 0, cam_eye_z = 10;

int key_pressed[256];

/* jumping parameters */
float original_y = 0;
float translate_x = 0, translate_y = 0;
float move_y = 0.001, move_x = 10;

/* size of the player */
float cube_size = 40;

/* rotation parameter */
float angle_param = 10;

/*  window width and height sizes */
float window_width, window_height;

/* animation parameters */
int start_animation = 0;
int jump_up = 0;


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
            if(!start_animation){
                start_animation = 1;
                glutTimerFunc(TIMER_INT, on_timer, TIMER0_ID);
            } else {
                start_animation = 0;
            }
            break;
        /* jump up */
        case 'W':
        case 'w':
            if(start_animation){
                if(!jump_up){
                    jump_up = 1;
                    glutTimerFunc(TIMER_INT, on_timer_jumping, TIMER1_ID);
                }
            }
            break;
        /* go left */
        case 'a':
        case 'A':
            if(start_animation){
                key_pressed['a'] = 1;
                glutTimerFunc(TIMER_INT, on_timer, TIMER0_ID);
            }
            break;
        /* go right */
        case 'd':
        case 'D':
            if(start_animation){
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
    window_height = height;
    window_width = width;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-width/2, width/2, -height/2, height/2, -100, 100);
    glMatrixMode(GL_MODELVIEW);
}

void on_timer(int value)
{
    if(value != TIMER0_ID){
        return;
    }

    glutPostRedisplay();

    if(start_animation){
        start_animation = 1;
        glutTimerFunc(TIMER_INT, on_timer, TIMER0_ID);
    }
}

void on_timer_jumping(int value)
{
    if(value != TIMER1_ID) {
        return;
    }

 /* increment the rotation parameter */
    angle_param += 5%360;

    if(jump_up){
        translate_y = 110*sin(move_y);
        move_y += 0.05;

        if(move_y >= 3.14){
            move_y = 0;
        }

        if(translate_y <= original_y){
            move_y = 0.001;
            jump_up = 0;
        }
    }

    if(jump_up){
        glutTimerFunc(TIMER_INT, on_timer_jumping, TIMER1_ID);
    }
}

void on_display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(
        cam_eye_x, cam_eye_y, cam_eye_z,
        0, 0, 0,
        0, 1, 0
    );

    initLighting();

    glPushMatrix();
    glTranslatef(translate_x, translate_y, 0);
    glRotatef(angle_param, 0, 1, 0);
    move();
    drawPlayer();
    glPopMatrix();

    glutSwapBuffers();
}
