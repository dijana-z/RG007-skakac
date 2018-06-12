#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <GL/glut.h>
#include "callbacks.h"
#include "functions.h"

#define SPACE 32
#define ESC 27
#define MAX_KEYS 256
#define MAX_PLATFORMS 9
#define MAX_CHAR 40
#define HIGHSCORE 256

#define TIMER_INT 10
#define TIMER0_ID 0

/* camera position parameters */
float cam_eye_x = 0, cam_eye_y = 0, cam_eye_z = 10;

/* initial moving platform parameters */
int min_width = 200;
int platform_size = 15;
float moving_prob = 0.3;
int platform_dist = 85;
int coin_size = 12;
int platform_rotation = 25;
int max_dist = 130, set_dist = 25;

/* platform movement parameters */
float pl_move_y = 0.5, pl_move_val = 0.5;

int level_no = 1, lives = 1;

/* coin parameters */
float coin_param = 15, delta_coin = 0.2, delta_c_rot = 1;
int collected_sum = 0, score = 0, life_needed = 15;
int collected_coins = 0, coin_width = 3, coin_rotation = 120;
float coin_prob = 0.2;
int additional_coins = 0;
int coin_no = 0, coins_needed = 5;
int max_c_mov = 22, min_c_mov = 15, coin_lines = 20;
int key_pressed[MAX_KEYS];

/* jumping parameters */
float translate_x = 0, translate_y = 0;
float move_y = 0.001, move_x = 6, helping_par = 5;
float delta_jump = 0.08, delta_angle = 5 % 360;
float gravity = 1;

/* initial player ground platform */
int ground = 0;

/* rotation parameter */
float angle_param = 10;

/* window width and height sizes */
float window_width = 800, window_height = 800;

/* animation parameters */
int start_animation = 0;
int start_screen = 1, pause_text = 0;
int jump_up = 0, falling = 0;
int first_jump = 0, game_over = 0;

/* highscores */
int hs1 = 0, hs2 = 0, hs3 = 0;

/* used to indicate whether the ground platform should be drawn */
int start = 1;

/* texture parameters */
GLuint platform_fd, ground_fd, background_fd, top_fd, top_ground_fd;
Point text_coords_top_gr[] = {
    {0, 0, 0},
    {1, 0, 0},
    {1, 1, 0},
    {0, 1, 0}
};
Point text_coords_front_gr[] = {
    {0, 0, 0},
    {2.5, 0, 0},
    {2.5, 0.1, 0},
    {0, 0.1, 0},
};
Point text_coords_front[] = {
    {0, 0, 0},
    {0.5, 0, 0},
    {0.5, 0.1, 0},
    {0, 0.1, 0},
};
Point text_coords_top[] = {
    {0, 0, 0},
    {0.02, 0, 0},
    {0.02, 0.02, 0},
    {0, 0.2, 0},
};

/* text parameters */
char points[MAX_CHAR], lives_left[MAX_CHAR], level_number[MAX_CHAR], highscore[HIGHSCORE];

/* player, platforms, and coins */
Player player;
Platform platforms[MAX_PLATFORMS];
Coin coins[MAX_PLATFORMS];


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
            if(!start_animation && !game_over) {
                start_animation = 1;
                start_screen = 0;
                pause_text = 0;
                glutTimerFunc(TIMER_INT, on_timer, TIMER0_ID);
            } else if (game_over) {
                /* restarting the game and reseting the parameters */
                first_jump = 0;
                falling = 0;
                jump_up = 0;
                start = 1;
                start_screen = 1;
                game_over = 0;
                pause_text = 0;
                lives = 1;
                level_no = 1;
                collected_sum = 0;
                collected_coins = 0;
                coins_needed = 5;
                score = 0;
                coin_prob = 0.2;
                pl_move_y = pl_move_val = 0.5;
                gravity = 1;
                move_y = 0.001;

                set_the_text();
                init_platforms();
                init_coordinates();
                glutPostRedisplay();
            } else {
                /* pausing the game */
                start_animation = 0;
                if(!pause_text) {
                    pause_text = 1;
                }
            }
            break;
        /* jump */
        case 'W':
        case 'w':
            if(start_animation) {
                if(!jump_up){
                    if(!first_jump) {
                        first_jump = 1;
                    }
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
            /* stop moving left */
            key_pressed['a'] = 0;
            break;
        case 'd':
        case 'D':
            /* stop moving right */
            key_pressed['d'] = 0;
                break;
    }
}

void on_reshape(int width, int height)
{
    /* set the window width and height parameters */
    window_height = height;
    window_width = width;

    /* init platform coordinates according to new window width/height and update player position */
    init_platforms();
    update_player();

    /* set the viewport */
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

    /* if the game isn't paused keep the animation running */
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

    glPushMatrix();
        set_the_text();
        /* set the potential ground for the player */
        first_ground();
        /* check if the player collected any coin */
        coin_collision();
        /* check if player landed on a platform */
        collision_check();
        /* draw and move the player if needed */
        draw_player();
        fall();
        jump();
        move();
    glPopMatrix();

    glPushMatrix();
        /* draw moving platforms and move them if needed */
        draw_platforms();
        start_moving();
        move_platforms();
        /* draw the coins */
        draw_coins();
    glPopMatrix();

    background_texture();

    glutSwapBuffers();
}
