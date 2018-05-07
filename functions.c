#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <GL/glut.h>
#include "callbacks.h"
#include "functions.h"

#define MAX_PLATFORMS 9
#define MAX_COINS 4
#define M_PI 3.1412

extern int start_animation, jump_up, falling, start;
extern float window_width, window_height;
extern float cube_size;
extern float translate_x, move_x;
extern float move_y, translate_y;
extern int key_pressed[];
extern int min_width, platform_size, platform_dist, platform_rotation;
extern int ground;
extern float moving_prob;
extern int level_no, collected_coins, coin_width, coin_rotation;
extern int coin_lines, max_c_mov, min_c_mov;
extern int coin_size;
extern float gravity, helping_par, angle_param, x;
extern float delta_jump, delta_angle;
extern float coin_param, delta_coin;

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

/* initialize players start coordinates */
void init_coordinates(void)
{
    /* initialize player state */
    player.size = 40;
    player.x_position = 0;
    player.y_position = platforms[ground].y_position + player.size/2 + platform_size/2;
    player.ground = 0;
}

/* set the platforms coordinates and parameters */
void init_platforms(void)
{
    if(start) {
        platforms[ground].y_position = -window_height/2 + platform_size/2;
        platforms[ground].pl_no = 0;
        platforms[ground].has_coin = 0;
        platforms[ground].scale_param = window_width/platform_size;
        platforms[ground].width = window_width;
    }

    for(int i = 1; i < MAX_PLATFORMS; ++i) {
        platforms[i].scale_param = ((rand() % ((int)window_width/2 - min_width + 1)) + min_width)/platform_size;
        platforms[i].width = platforms[i].scale_param*platform_size;
        platforms[i].has_coin = 0;
        platforms[i].pl_no = i;
        platforms[i].move = (rand()/(float)RAND_MAX < moving_prob) ? (rand() % 2 + 1) : 0;

        float   min_val = platforms[i].width/2 - window_width/2,
                initial_trans = -window_height/2 + 2*platform_size + platform_size/2;

        platforms[i].x_position = (rand() % (int)(window_width - platforms[i].width + 1)) + min_val;
        platforms[i].y_position = initial_trans + platforms[i].pl_no*platform_dist;
    }

    for(int i = 0; i < level_no; ++i) {
        coins = realloc(coins, i*sizeof(*coins));
        if(NULL == coins) {
            fprintf(stderr, "Coin reallocation\n");
            exit(EXIT_FAILURE);
        }

        int rand_plat = rand() % 7 + 1;
        coins[i].x_position = platforms[rand_plat].x_position;
        coins[i].start_y = platforms[rand_plat].y_position;
        coins[i].is_visible = 1;
    }
}

/* update players position */
void update_player(void)
{
    player.x_position = platforms[player.ground].x_position;
    player.y_position = platforms[player.ground].y_position + platform_size/2 + player.size/2;
}

/* draw the player */
void draw_player(void)
{
    glColor3f(1, 0, 0);
    glTranslatef(player.x_position, player.y_position, 0);
    glRotatef(angle_param, 0, 1, 0);
    glutSolidCube(player.size);
}

void draw_ground(void)
{
    glPushMatrix();
        glColor3f(0, 0.5, 0.7);
        glTranslatef(platforms[ground].x_position, platforms[ground].y_position, -player.size/2);
        glRotatef(platform_rotation, 1, 0, 0);
        glScalef(platforms[ground].scale_param, 1, 1);
        glutSolidCube(platform_size);
    glPopMatrix();
}

/* draw moving platforms */
void draw_platforms(void)
{
    if(start) {
        draw_ground();
    }

    for(int i = 1; i < MAX_PLATFORMS; ++i) {
        glPushMatrix();
            glColor3f(0.9, 0.5, 0.7);
            glTranslatef(platforms[i].x_position, platforms[i].y_position, -player.size/2);
            glRotatef(platform_rotation, 1, 0, 0);
            glScalef(platforms[i].scale_param, 1, 1);
            glutSolidCube(platform_size);
        glPopMatrix();
    }
}

/* draw the coins on platforms */
void draw_coins(void)
{
    for(int i = 0; i < level_no; ++i) {
        if(coins[i].is_visible) {
            coins[i].y_position = coins[i].start_y + coin_size + coin_param;

            /* draw the top disc */
            glPushMatrix();
                glColor3f(1, 1, 0);
                glTranslatef(coins[i].x_position, coins[i].y_position, coin_width);
                glRotatef(coin_rotation, 0, 1, 0);
                gluDisk(gluNewQuadric(), 0, coin_size, coin_lines, coin_lines);
            glPopMatrix();

            /* draw the cylinder */
            glPushMatrix();
                glColor3f(1, 1, 0);
                glTranslatef(coins[i].x_position, coins[i].y_position, coin_width);
                glRotatef(coin_rotation, 0, 1, 0);
                gluCylinder(gluNewQuadric(), coin_size, coin_size, coin_width, coin_lines, coin_lines);
            glPopMatrix();

            /* draw the bottom disc */
            glPushMatrix();
                glColor3f(1, 1, 0);
                glTranslatef(coins[i].x_position, coins[i].y_position, 0);
                glRotatef(coin_rotation, 0, 1, 0);
                gluDisk(gluNewQuadric(), 0, coin_size, coin_lines, coin_lines);
            glPopMatrix();
        }
    }

    coin_param += delta_coin;
    if(coin_param >= max_c_mov || coin_param <= min_c_mov) {
        delta_coin *= -1;
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
                if(i == player.ground) {
                    player.x_position += platforms[i].move;
                }
            } else {
                 platforms[i].move *= -1;
            }
        }
    }
}

/* x-axis movement function for player */
void move(void)
{
    if(key_pressed['a']) {
        if(player.x_position - move_x - helping_par >= -window_width/2 + player.size/2) {
            player.x_position -= move_x;

            if(!jump_up && platforms[player.ground].x_position - platforms[player.ground].width/2 >= player.x_position) {
                printf("PADA\n");
                falling = 1;
                fall();
            }
        }
    } else if(key_pressed['d']) {
        if(player.x_position + move_x + helping_par <= window_width/2 - player.size/2) {
            player.x_position += move_x;

            if(!jump_up && platforms[player.ground].x_position + platforms[player.ground].width/2 <= player.x_position) {
                printf("PADA\n");
                falling = 1;
                fall();
            }
        }
    }
}

/* jumping */
void jump(void)
{
    if(jump_up  && !falling) {
        angle_param += delta_angle;
        player.y_position = 130*sin(move_y) + platforms[player.ground].y_position + platform_size/2 + player.size/2;

        move_y += delta_jump;

        if(move_y >= M_PI) {
            move_y = 0.001;
            jump_up = 0;

            /* if we jumped on a platform stay there */
            for(int i = 0; i < MAX_PLATFORMS; ++i) {
                if(get_collision(get_box(platforms[i]))) {
                    return;
                }
            }

            /* after jumping the player isn't on the platform, so he starts falling */
            falling = 1;
            fall();
        }
    }
}

/* falling */
void fall(void)
{
    if(falling) {
        player.y_position -= x*gravity;
        x += 0.4;

        if(player.y_position + player.size/2 <= -window_height/2) {
            game_over();
        }
    }
}

/* get the box around platform for collision detecting */
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

Box coin_box(Coin coin)
{
    Box box = {
        .y_top = coin.y_position + coin_size/2,
        .y_bottom = coin.y_position - coin_size/2,
        .x_left = coin.x_position - coin_size/2,
        .x_right = coin.x_position + coin_size/2
    };

    return box;
}

/* check if player collided with platform */
bool get_collision(Box box)
{
    float   player_head = player.y_position + player.size/2,
            player_bottom = player.y_position - player.size/2,
            player_left = player.x_position,
            player_right = player.x_position;

    if(player_head > box.y_top && player_bottom < box.y_top &&
        player_left > box.x_left && player_right < box.x_right) {
        return true;
    }

    return false;
}

/* set the last platform that player passed as pot base */
void first_ground(void)
{
    for(int i = 0; i < MAX_PLATFORMS; ++i) {
        if(player.y_position >= platforms[i].y_position + platform_size/2 + player.size) {
            player.pot_base = i;
        } else {
            break;
        }
    }
}

/* check if there was a collision between player and platforms */
void collision_check(void)
{
    if((jump_up && move_y >= M_PI/2) || falling) {
        for(int i = 0; i < MAX_PLATFORMS; ++i) {
            if(get_collision(get_box(platforms[i]))) {
                if(player.pot_base <= i) {
                    player.ground = i;
                    player.y_position = platforms[i].y_position + platform_size/2 + player.size/2;
                    move_y = 0.001;
                    jump_up = 0;
                    falling = 0;
                    x = 3.5;
                    printf("igrac je na: %d\n", player.ground);
                }
            }
        }
    }
}

bool coin_coll_check(Box box)
{
    float   player_head = player.y_position + player.size/2,
            player_bottom = player.y_position - player.size/2,
            player_left = player.x_position - player.size/2,
            player_right = player.x_position + player.size/2;

    if(player_head >= box.y_top && player_bottom <= box.y_top
    && player_right >= box.x_left && player_left <= box.x_right) {
        return true;
    }

    return false;
}

/* collecting the coins */
void coin_collision(void)
{
    for(int i = 0; i < level_no; ++i) {
        if(coins[i].is_visible) {
            if(coin_coll_check(coin_box(coins[i]))) {
                collected_coins++;
                coins[i].is_visible = 0;
                printf("Coins: %d\n", collected_coins);
            }
        }
    }
}

/* print message and exit the game */
void game_over(void)
{
// TODO: ispisi game over
    printf("GAME OVER\n");
    free(coins);
    exit(EXIT_SUCCESS);
}
