#define _XOPEN_SOURCE 700

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <GL/glut.h>
#include "callbacks.h"
#include "functions.h"
#include "image.h"

#define MAX_PLATFORMS 9
// #define M_PI 3.141592653589793
#define BACKGROUND_TEXTURE "./textures/pozadina.bmp"
#define PLATFORM_TEXTURE "./textures/najsvetlija.bmp"
#define GROUND_TEXTURE "./textures/svetlije.bmp"
#define TOP_TEXTURE "./textures/ceil.bmp"
#define TOP_GROUND_TEXTURE "./textures/gore.bmp"


extern int start_animation, jump_up, falling, start, first_jump;
extern int start_screen, game_over, pause_text;
extern float window_width, window_height;
extern float cube_size;
extern float translate_x, move_x;
extern float move_y, translate_y;
extern int key_pressed[];
extern int min_width, platform_size, platform_dist, platform_rotation;
extern int ground, max_dist, set_dist;
extern float moving_prob;
extern int level_no, collected_coins, coin_width, coin_rotation;
extern float coin_prob;
extern int coin_no, additional_coins;
extern int coin_lines, max_c_mov, min_c_mov;
extern int coin_size, lives, coins_needed;
extern int score, collected_sum, life_needed;
extern float gravity, helping_par, angle_param;
extern float delta_jump, delta_angle;
extern float coin_param, delta_coin, delta_c_rot;
extern float pl_move_y, pl_move_val;
extern int hs1, hs2, hs3;
extern char points[], lives_left[], level_number[], highscore[];
extern GLuint platform_fd, ground_fd, background_fd, top_fd, top_ground_fd;
extern Point text_coords_top_gr[], text_coords_front_gr[];
extern Point text_coords_top[], text_coords_front[];

const int indices_front[] = {0, 1, 2, 3, 0};
const int indices_top[] = {3, 2, 4, 5, 3};

extern Platform platforms[];
extern Player player;
extern Coin coins[];


/* initialize lighting */
void init_lighting(void)
{
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    /* initialize light and shininess parameters */
    GLfloat light_position[]  = { 1, 1, 1, 0 };
    GLfloat light_ambient[]   = { 0.0, 0.0, 0.0, 1 };
    GLfloat light_diffuse[]   = { 0.7, 0.7, 0.7, 1 };
    GLfloat light_specular[]  = { 0.9, 0.9, 0.9, 1 };
    GLfloat ambient_coeffs[]  = { 1.0, 0.1, 0.1, 1 };
    GLfloat diffuse_coeffs[]  = { 0.0, 0.0, 0.8, 1 };
    GLfloat specular_coeffs[] = { 1, 1, 1, 1 };
    GLfloat shininess = 40;

    /* set the lighting */
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    /* set the material */
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

/* initialize players start coordinates */
void init_coordinates(void)
{
    /* initial player state */
    player.size = 40;
    player.x_position = 0;
    player.ground = ground;
    player.pot_base = ground;
    player.y_position = platforms[ground].y_position + player.size/2 + platform_size/2;
}

/* initialize textures */
void initialize_texture(void)
{
    background_fd = generate_texture(BACKGROUND_TEXTURE);
    platform_fd = generate_texture(PLATFORM_TEXTURE);
    ground_fd = generate_texture(GROUND_TEXTURE);
    top_fd = generate_texture(TOP_TEXTURE);
    top_ground_fd = generate_texture(TOP_GROUND_TEXTURE);
}

/* set the platforms coordinates and parameters */
void init_platforms(void)
{
    coin_no = 0;

    /* if the game just started and the player didn't go up, set the ground platform parameters */
    if(start) {
        platforms[ground].y_position = -window_height/2 + platform_size/2;
        platforms[ground].pl_no = 0;
        platforms[ground].has_coin = 0;
        platforms[ground].scale_param = window_width/platform_size;
        platforms[ground].width = window_width;
        platforms[ground].x_position = 0;
    }

    /* set the parameters for the rest of the platforms */
    for(int i = 1; i < MAX_PLATFORMS; ++i) {
        /* generating random number between window_width/2 and min_width for scaling */
        platforms[i].scale_param = ((rand() % ((int)window_width/2 - min_width + 1)) + min_width)/platform_size;

        /* setting the width of the platform using that random scale number we just generated */
        platforms[i].width = platforms[i].scale_param*platform_size;
        platforms[i].pl_no = i;

        /* generating random moving speed if the platform needs to move */
        platforms[i].move = (rand()/(float)RAND_MAX < moving_prob) ? (rand() % 2 + 1) : 0;

        float   min_val = platforms[i].width/2 - window_width/2,
                initial_trans = -window_height/2 + 2*platform_size + platform_size/2;

        /* generating random x position for the platform */
        platforms[i].x_position = (rand() % (int)(window_width - platforms[i].width + 1)) + min_val;

        /* setting y position for the platform based on the initial distance from the ground and the platform number */
        platforms[i].y_position = initial_trans + platforms[i].pl_no*platform_dist;

        /* setting the coin parameter for the platform */
        platforms[i].has_coin = (rand()/(float)RAND_MAX < coin_prob) ? 1 : 0;

        /* checking if the platforms aren't to far from each other for jumping */
        if(i > 1) {
            if(platforms[i-1].x_position < platforms[i].x_position) {
                /* check if the adjusting is needed */
                if((platforms[i].x_position - platforms[i].width/2) - (platforms[i-1].x_position + platforms[i-1].width/2) > max_dist) {
                    platforms[i].x_position = platforms[i-1].x_position + platforms[i-1].width/2 + set_dist;

                    /* if the platform is now out of the window range, adjust it just a little bit more */
                    if(platforms[i].x_position + platforms[i].width/2 > window_width/2) {
                        platforms[i].x_position -= platforms[i].x_position + platforms[i].width/2 - window_width/2;
                    }
                }
            } else {
                /* check if the adjusting is needed */
                if((platforms[i-1].x_position - platforms[i-1].width/2) - (platforms[i].x_position + platforms[i].width/2) > max_dist) {
                    platforms[i-1].x_position = platforms[i].x_position + platforms[i].width/2 + set_dist;

                    /* if the platform is now out of the window range, adjust it just a little bit more */
                    if(platforms[i-1].x_position + platforms[i-1].width/2 > window_width/2) {
                        platforms[i-1].x_position -= platforms[i-1].x_position + platforms[i-1].width/2 - window_width/2;
                    }
                }
            }
        }
    }

    /* setting the coins */
    for(int i = 0; i < MAX_PLATFORMS; ++i) {
        /* generate random platform for the coin */
        if(platforms[i].has_coin) {
          /* set the coin position */
          coins[coin_no].pl_no = platforms[i].pl_no;
          coins[coin_no].x_position = platforms[i].x_position;
          coins[coin_no].start_y = platforms[i].y_position;
          coin_no++;
        }
    }
}

void platform_texture(int i)
{
    /* get vertices of the platform */
    Box box = get_box(platforms[i]);
    float z_near = -player.size/2;
    float z_far = -player.size/2 - platform_size;
    float x_left = box.x_left;
    float x_right = box.x_right;
    float y_top = box.y_top;
    float y_bottom = box.y_bottom;

    Point coords[] = {
        {x_left - platform_size/2, y_bottom, z_near},
        {x_right - platform_size/2, y_bottom, z_near},
        {x_right - platform_size/2, y_top, z_near},
        {x_left - platform_size/2, y_top, z_near},
        {x_right - platform_size/2 - 10, y_top + platform_size/2, z_far},
        {x_left - platform_size/2 + 10, y_top + platform_size/2, z_far}
    };

    /* draw texture for the front part of the platform */
    glBindTexture(GL_TEXTURE_2D, platform_fd);
    glBegin(GL_QUADS);
    for(int j = 0; j < (int)sizeof(indices_front)/(int)sizeof(int); ++j) {
        glNormal3f(coords[indices_front[j]].x, coords[indices_front[j]].y, coords[indices_front[j]].z);
        glTexCoord2f(text_coords_front[indices_front[j]].x, text_coords_front[indices_front[j]].y);
        glVertex3f(coords[indices_front[j]].x, coords[indices_front[j]].y, coords[indices_front[j]].z);
    }
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);

    /* draw texture for the top part of the platform */
    glBindTexture(GL_TEXTURE_2D, top_fd);
    glBegin(GL_QUADS);
    for(int j = 0; j < (int)sizeof(indices_top)/(int)sizeof(int); ++j) {
        glNormal3f(coords[indices_top[j]].x, coords[indices_top[j]].y, coords[indices_top[j]].z);
        glTexCoord2f(text_coords_top[indices_front[j]].x, text_coords_top[indices_front[j]].y);
        glVertex3f(coords[indices_top[j]].x, coords[indices_top[j]].y, coords[indices_top[j]].z);
    }
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ground_texture(void)
{
    /* get vertices of the ground platform */
    Box box = get_box(platforms[ground]);
    float z_near = -player.size/2;
    float z_far = -player.size/2 - platform_size;
    float x_left = box.x_left;
    float x_right = box.x_right;
    float y_top = box.y_top;
    float y_bottom = box.y_bottom;

    Point coords[] = {
        {x_left - platform_size/2, y_bottom, z_near},
        {x_right, y_bottom, z_near},
        {x_right, y_top, z_near},
        {x_left - platform_size/2, y_top, z_near},
        {x_right - platform_size/2, y_top + platform_size/2, z_far},
        {x_left + platform_size/2, y_top + platform_size/2, z_far}
    };

    /* draw texture for the front part of the platform */
    glBindTexture(GL_TEXTURE_2D, ground_fd);
    glBegin(GL_QUADS);
    for(int j = 0; j < (int)sizeof(indices_front)/(int)sizeof(int); ++j) {
        glNormal3f(coords[indices_front[j]].x, coords[indices_front[j]].y, coords[indices_front[j]].z);
        glTexCoord2f(text_coords_front_gr[indices_front[j]].x, text_coords_front_gr[indices_front[j]].y);
        glVertex3f(coords[indices_front[j]].x, coords[indices_front[j]].y, coords[indices_front[j]].z);
    }
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);

    /* draw texture for the top part of the platform */
    glBindTexture(GL_TEXTURE_2D, top_ground_fd);
    glBegin(GL_QUADS);
    for(int j = 0; j < (int)sizeof(indices_top)/(int)sizeof(int); ++j) {
        glNormal3f(coords[indices_top[j]].x, coords[indices_top[j]].y, coords[indices_top[j]].z);
        glTexCoord2f(text_coords_top_gr[indices_top[j]].x, text_coords_top_gr[indices_top[j]].y);
        glVertex3f(coords[indices_top[j]].x, coords[indices_top[j]].y, coords[indices_top[j]].z);
        glRotatef(platform_rotation, 1, 0, 0);
    }
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void background_texture(void)
{
    glEnable(GL_TEXTURE_2D);

    /* get vertices of the screen */
    float x_left = -window_width/2;
    float x_right = window_width/2;
    float y_top = window_height/2;
    float y_bottom = -window_height/2;
    float z_coord = -50;

    Point coords[] = {
        {x_left - 50, y_bottom, z_coord},
        {x_right, y_bottom, z_coord},
        {x_right, y_top, z_coord},
        {x_left - 50, y_top, z_coord}
    };

    Point text_coords[] = {
        {0, 0, 0},
        {1, 0, 0},
        {1, 1, 0},
        {0, 1, 0},
    };

    int indices[] = {0, 1, 2, 3, 0};

    /* draw the background platform */
    glBindTexture(GL_TEXTURE_2D, background_fd);
    glBegin(GL_QUADS);
    for(int j = 0; j < (int)sizeof(indices)/(int)sizeof(int); ++j) {
        glNormal3f(coords[indices[j]].x, coords[indices[j]].y, coords[indices[j]].z);
        glTexCoord2f(text_coords[indices[j]].x, text_coords[indices[j]].y);
        glVertex3f(coords[indices[j]].x, coords[indices[j]].y, coords[indices[j]].z);
    }
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
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
    if(game_over) {
        player.y_position = -window_height/2 - player.size;
    }

    glColor3f(1, 0, 0);
    glTranslatef(player.x_position, player.y_position, 0);
    glRotatef(angle_param, 0, 1, 0);
    glutSolidCube(player.size);
}

/* draw the ground platform */
void draw_ground(void)
{
    glPushMatrix();
        glColor3f(0, 0.5, 0.7);
        glTranslatef(platforms[ground].x_position, platforms[ground].y_position, -player.size/2);
        glRotatef(platform_rotation, 1, 0, 0);
        glScalef(platforms[ground].scale_param, 1, 1);
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);
    ground_texture();
    glDisable(GL_TEXTURE_2D);
}

/* draw moving platforms and ground platform if needed */
void draw_platforms(void)
{
    int i;
    if(start) {
        draw_ground();
        i = 1;
    } else {
        i = 0;
    }

    for(; i < MAX_PLATFORMS; ++i) {
        glPushMatrix();
            glColor3f(0.9, 0.5, 0.7);
            glTranslatef(platforms[i].x_position, platforms[i].y_position, -player.size/2);
            glRotatef(platform_rotation, 1, 0, 0);
            glScalef(platforms[i].scale_param, 1, 1);
        glPopMatrix();

        glEnable(GL_TEXTURE_2D);
        platform_texture(i);
        glDisable(GL_TEXTURE_2D);
    }
}

/* draw the coins on platforms */
void draw_coins(void)
{
    for(int i = 0; i < coin_no; ++i) {
        /* if player didn't collect that coin, set it's y position and draw the coin */
        coins[i].y_position = coins[i].start_y + coin_size + coin_param;

        GLUquadric *quad = gluNewQuadric();

        /* draw the coin */
        glPushMatrix();
            glColor3f(1, 1, 0);
            glTranslatef(coins[i].x_position, coins[i].y_position, 0);
            glRotatef(coin_rotation, 0, 1, 0);
            gluCylinder(quad, coin_size, coin_size, coin_width, coin_lines, coin_lines);
            gluDisk(quad, 0, coin_size, coin_lines, coin_lines);
            glTranslatef(0, 0, coin_width);
            gluDisk(quad, 0, coin_size, coin_lines, coin_lines);
        glPopMatrix();

        gluDeleteQuadric(quad);
    }

    /* change the rotation parameter of the coins */
    coin_rotation += delta_c_rot;

    if(coin_rotation >= 360) {
        coin_rotation = 0;
    }

    /* move the coins up and down */
    coin_param += delta_coin;
    if(coin_param >= max_c_mov || coin_param <= min_c_mov) {
        delta_coin *= -1;
    }
}

/* **********************************************************************************
 * display the text on the screen
 * code from http://www.codersource.net/2011/01/27/displaying-text-opengl-tutorial-5/
 * **********************************************************************************/
void text_display(char *str, float x, float y, float z)
{
	glRasterPos3f(x, y,z);

	for(int i = 0; str[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
	}
}

/* set the text parameters and print it on the screen */
void set_the_text(void)
{
    glDisable(GL_LIGHTING);
    if(start_screen && !game_over && !pause_text) {
        /* Before the start, print the greeting message */
        glColor3f(1, 1, 1);
        text_display("Press SPACE to start the game.", -140, 0, 30);
        text_display("To move the player press W, A or D.", -165, -50, 30);
    }
    if(pause_text && !game_over && !start_screen) {
        glColor3f(1, 1, 1);
        text_display("Paused! Press SPACE to continue.", -140, 0, 30);
    }
    if(!start_screen && !pause_text && !game_over) {
        /* make new strings for collected coins, lives and level_no */
        sprintf(points, "Your score: %d", score);
        sprintf(lives_left, "Lives left: %d, collect %d more coins!", lives, life_needed - collected_sum);
        sprintf(level_number, "Level: %d, collect %d more coins!", level_no, coins_needed - collected_coins);

        /* print the strings on the screen */
        glColor3f(1, 1, 1);
        text_display(points, -window_width/2+20, window_height/2 - 20, 30);
        text_display(lives_left, -window_width/2+20, window_height/2 - 40, 30);
        text_display(level_number, -window_width/2+20, window_height/2 - 60, 30);
    }
    if(game_over && !start_screen && !pause_text) {
       highscores();
       /* if the game is over, print the goodbye message */
       glColor3f(1, 1, 1);
       text_display("Game Over! Press ESC to exit.", -140, 0, 30);
       text_display("Press SPACE to restart the game.", -150, -50, 30);
       text_display(highscore, -190, -100, 30);
   }
   glEnable(GL_LIGHTING);
}

/* print and update highscores */
void highscores(void)
{
	/* opening the file and creating new if it doesn't exist */
	int fd = open("highscores.txt", O_CREAT | O_RDWR, 0600);

	if(fd == -1) {
		fprintf(stderr, "Opening file for highscores failed\n");
		exit(EXIT_FAILURE);
	}

	FILE *f = fdopen(fd, "r+");
	if(f == NULL) {
		fprintf(stderr, "Fdopen failed\n");
		exit(EXIT_FAILURE);
	}

	/* check if the file existed */
	struct stat fInfo;
	if(fstat(fd, &fInfo) == -1) {
		fprintf(stderr, "Fstat failed\n");
		exit(EXIT_FAILURE);
	}

	int file_size = fInfo.st_size;

    /* if the file was just created generate three highscores */
	if(!file_size) {
		hs1 = 50;
		hs2 = 30;
        hs3 = 10;
	} else {
		fscanf(f, "%d %d %d", &hs1, &hs2, &hs3);
	}

    int changed = 0;

    /* update the highscores */
	if(score > hs1) {
		hs3 = hs2;
		hs2 = hs1;
		hs1 = score;
        changed = 1;
	} else if(score > hs2) {
		hs3 = hs2;
		hs2 = score;
        changed = 1;
	} else if(score > hs3) {
		hs3 = score;
        changed = 1;
	}

	/* erase the previous highscores */
	if(ftruncate(fd, 0) == -1) {
		fprintf(stderr, "Ftruncate failed\n");
		exit(EXIT_FAILURE);
	}

    /* reset the file offset */
	if(lseek(fd, 0, SEEK_SET) == -1) {
		fprintf(stderr, "Lseek failed\n");
		exit(EXIT_FAILURE);
	}

	fprintf(f, "%d %d %d", hs1, hs2, hs3);
    if(changed) {
        sprintf(highscore, "New highscore!\nHighscores:\n1. %d\n2. %d\n3. %d", hs1, hs2, hs3);
    } else {
        sprintf(highscore, "Try again!\nHighscores:\n1. %d\n2. %d\n3. %d", hs1, hs2, hs3);
    }

	fclose(f);
	close(fd);
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

                if(platforms[i].has_coin) {
                    for(int j = 0; j < coin_no; ++j) {
                        if(coins[j].pl_no == platforms[i].pl_no) {
                            coins[j].x_position += platforms[i].move;
                        }
                    }
                }
            } else {
                 platforms[i].move *= -1;
            }
        }
    }
}

/* y-axis movement function for the world */
void start_moving(void)
{
    if(first_jump) {
        for(int i = 0; i < MAX_PLATFORMS; ++i) {
            /* move platforms */
            platforms[i].y_position -= pl_move_y;

            /* rotate platforms */
            if(platforms[i].y_position + platform_size/2 <= -window_height/2) {
                if(player.ground == i) {
                    if(!jump_up) {
                        player.y_position = -window_height/2 - player.size;
                        lives--;

                        if(lives <= 0) {
                            game_over = 1;
                            start_animation = 0;
                            start_screen = 0;
                            pause_text = 0;
                            return;
                        } else {
                            player.ground = 3;
                            player.y_position = platforms[player.ground].y_position + platform_size/2 + player.size/2;
                            player.x_position = platforms[player.ground].x_position;
                            start_animation = 0;
                            start_screen = 1;
                        }
                    }
                }

                player.ground--;

                if(platforms[i-1].has_coin) {
                    for(int k = i+1; k < coin_no; ++k) {
                        coins[k-1].pl_no = coins[k].pl_no;
                        coins[k-1].y_position = coins[k].y_position;
                        coins[k-1].x_position = coins[k].x_position;
                        coins[k-1].start_y = coins[k].start_y;
                    }

                    coin_no --;
                }

                int j = 0, k = i+1;

                if(start) {
                    start = 0;
                }

                for(; k < MAX_PLATFORMS; ++k, ++j) {
                    platforms[j].width = platforms[k].width;
                    platforms[j].scale_param = platforms[k].scale_param;
                    platforms[j].x_position = platforms[k].x_position;
                    platforms[j].y_position = platforms[k].y_position;
                    platforms[j].move = platforms[k].move;
                    platforms[j].has_coin = platforms[k].has_coin;
                    platforms[j].pl_no = platforms[k].pl_no;
                }

                for(; j < MAX_PLATFORMS; ++j) {
                    float min_val = platforms[j].width/2 - window_width/2;

                    platforms[j].scale_param = ((rand() % ((int)window_width/2 - min_width + 1)) + min_width)/platform_size;
                    platforms[j].width = platforms[j].scale_param*platform_size;
                    platforms[j].pl_no = platforms[j-1].pl_no + 1;
                    platforms[j].move = (rand()/(float)RAND_MAX < moving_prob) ? (rand() % 2 + 1) : 0;
                    platforms[j].x_position = (rand() % (int)(window_width - platforms[j].width + 1)) + min_val;
                    platforms[j].y_position = platforms[j-1].y_position + platform_dist;


                    if(platforms[j-1].x_position < platforms[j].x_position) {
                        /* check if the adjusting is needed */
                        if((platforms[j].x_position - platforms[j].width/2) - (platforms[j-1].x_position + platforms[j-1].width/2) > max_dist) {
                            platforms[j].x_position = platforms[j-1].x_position + platforms[j-1].width/2 + set_dist;

                            /* if the platform is now out of the window range, adjust it just a little bit more */
                            if(platforms[j].x_position + platforms[j].width/2 > window_width/2) {
                                platforms[j].x_position -= platforms[j].x_position + platforms[j].width/2 - window_width/2;
                            } else if(platforms[j].x_position - platforms[j].width/2 < -window_width/2) {
                                platforms[j].x_position += -window_width/2 - platforms[j].x_position - platforms[j].width/2;
                            }
                        }
                    } else {
                        /* check if the adjusting is needed */
                        if((platforms[j-1].x_position - platforms[j-1].width/2) - (platforms[j].x_position + platforms[j].width/2) > max_dist) {
                            platforms[j].x_position = platforms[j-1].x_position - platforms[j-1].width/2 - set_dist;

                            /* if the platform is now out of the window range, adjust it just a little bit more */
                            if(platforms[j].x_position + platforms[j].width/2 > window_width/2) {
                                platforms[j].x_position -= platforms[j].x_position + platforms[j].width/2 - window_width/2;
                            } else if(platforms[j].x_position - platforms[j].width/2 < -window_width/2) {
                                platforms[j].x_position += -window_width/2 - platforms[j].x_position - platforms[j].width/2;
                            }
                        }
                    }

                    /* setting the coin if needed */
                    platforms[j].has_coin = (rand()/(float)RAND_MAX < coin_prob) ? 1 : 0;
                    if(platforms[j].has_coin) {
                        coins[coin_no].pl_no = platforms[j].pl_no;
                        coins[coin_no].x_position = platforms[j].x_position;
                        coins[coin_no].start_y = platforms[j].y_position;
                        coin_no++;
                    }
                }
            }
        }

        /* move coins */
        for(int i = 0; i < coin_no; ++i) {
            coins[i].start_y -= pl_move_y;
        }

        /* if player stands on a platform, move him with the platform he stands on */
        if(!jump_up && !falling) {
            player.y_position = platforms[player.ground].y_position + platform_size/2 + player.size/2;
        }

        /* adjust the moving speed a bit to avoid unnecessary waiting at the top */
        if(player.y_position >= 300) {
            if(!additional_coins) {
                additional_coins = 1;
                score += 5;
            }
            pl_move_y = pl_move_val*4;
        } else if(player.y_position <= -window_height/2 + 400) {
            pl_move_y = pl_move_val;
            additional_coins = 0;
        }
    }
}

/* x-axis movement function for player */
void move(void)
{
    if(key_pressed['a']) {
        /* if possible, move left */
        if(player.x_position - move_x - helping_par >= -window_width/2 + player.size/2) {
            player.x_position -= move_x;

            /* if the player passed the platform and isn't still jumping then he starts to fall */
            if(!jump_up && platforms[player.ground].x_position - platforms[player.ground].width/2 >= player.x_position) {
                falling = 1;
                fall();
            }
        }
    } else if(key_pressed['d']) {
        /* if possible, move right */
        if(player.x_position + move_x + helping_par <= window_width/2 - player.size/2) {
            player.x_position += move_x;

            /* if the player passed the platform and isn't still jumping then he starts to fall */
            if(!jump_up && platforms[player.ground].x_position + platforms[player.ground].width/2 <= player.x_position) {
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
        /* change the rotation of the cube */
        angle_param += delta_angle;

        /* if there is no ground, keep falling */
        if(player.ground == -1) {
            falling = 1;
            jump_up = 0;
            fall();
            return;
        }

        /* set the new y position */
        player.y_position = 120*sin(move_y) + platforms[player.ground].y_position + platform_size/2 + player.size/2;

        /* change the jump parameter */
        move_y += delta_jump;

        /* if the jump is completed, set the jump_up parameter to 0 */
        if(move_y >= M_PI) {
            move_y = 0.001;
            jump_up = 0;

            /* if the player jumped on a platform, he should stay there */
            for(int i = 0; i < MAX_PLATFORMS; ++i) {
                if(get_collision(get_box(platforms[i]))) {
                    player.y_position = platforms[i].y_position + player.size/2 + platform_size/2;
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
        /* set the new y position for the player */
        player.y_position -= 120*delta_jump*gravity;
        gravity += 0.02;

        /* if the player fell through the floor, the game is over */
        if(player.y_position + player.size/2 <= -window_height/2) {
            lives--;

            if(lives <= 0) {
                game_over = 1;
                start_animation = 0;
                start_screen = 0;
                pause_text = 0;
                return;
            } else {
                player.ground = 3;
                player.y_position = platforms[player.ground].y_position + platform_size/2 + player.size/2;
                player.x_position = platforms[player.ground].x_position;
                start_animation = 0;
                start_screen = 1;
                game_over = 0;
            }
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

/* get the box around coin for collision detecting */
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

/* check if player collided with coin */
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

/* set the last platform that player passed as potential base platform */
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
            /* if the collision happened set the player position and jumping/falling parameters */
            if(get_collision(get_box(platforms[i]))) {
                if(player.pot_base <= i) {
                    player.ground = i;
                    player.y_position = platforms[i].y_position + platform_size/2 + player.size/2;
                    move_y = 0.001;
                    jump_up = 0;
                    falling = 0;
                    gravity = 1;
                }
            }
        }
    }
}

/* collecting the coins */
void coin_collision(void)
{
    for(int i = 0; i < coin_no; ++i) {
        /* if collision happened, collect the coin */
        if(coin_coll_check(coin_box(coins[i]))) {
            collected_coins++;
            collected_sum++;

            if(collected_sum == life_needed) {
                collected_sum = 0;
                if(lives < 3) {
                    lives++;
                }
            }

            score++;

            if(collected_coins == coins_needed) {
                level_upgrade();
            }

            /* set the rest of the coins */
            for(int k = i+1; k < coin_no; ++k) {
                coins[k-1].pl_no = coins[k].pl_no;
                coins[k-1].y_position = coins[k].y_position;
                coins[k-1].x_position = coins[k].x_position;
                coins[k-1].start_y = coins[k].start_y;
            }

            /* decrease the number of coins on the screen */
            coin_no --;
        }
    }
}

void level_upgrade(void)
{
    collected_coins = 0;

    /* speed up */
    pl_move_val += 0.6;
    if(pl_move_val > 2.6) {
        pl_move_val = 2.6;
    }
    pl_move_y = pl_move_val;

    /* increase coins needed for the next level and coin probability for platforms */
    coins_needed += 1;
    coin_prob += 0.05;

    level_no += 1;
}
