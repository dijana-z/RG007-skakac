#include <GL/glut.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "functions.h"

#define MAX_KEYS 256

extern float window_width, window_height;
extern float cube_size;
extern float translate_x, move_x;
extern int key_pressed[MAX_KEYS];
extern float rand_width1, rand_width2, rand_width3, rand_width4, rand_width5, rand_width6;

static float platform1_width = 0;
static float platform2_width = 0;
static float platform3_width = 0;
static float platform4_width = 0;
static float platform5_width = 0;
static float platform6_width = 0;


void initLighting(void)
{
    GLfloat light_position[] = { 1, 1, 1, 0 };
    GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1 };
    GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };
    GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };
    GLfloat ambient_coeffs[] = { 1.0, 0.1, 0.1, 1 };
    GLfloat diffuse_coeffs[] = { 0.0, 0.0, 0.8, 1 };
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

void initCoordinats(void)
{
    rand_width1 = 20 + rand()/(RAND_MAX / (window_width/5 - 10 + 1) + 1);
    rand_width2 = 20 + rand()/(RAND_MAX / (window_width/5 - 10 + 1) + 1);
    rand_width3 = 20 + rand()/(RAND_MAX / (window_width/5 - 10 + 1) + 1);
    rand_width4 = 20 + rand()/(RAND_MAX / (window_width/5 - 10 + 1) + 1);
    rand_width5 = 20 + rand()/(RAND_MAX / (window_width/5 - 10 + 1) + 1);
    rand_width6 = 20 + rand()/(RAND_MAX / (window_width/5 - 10 + 1) + 1);
}

void drawPlayer(void)
{
    glColor3f(1, 0, 0);
    glTranslatef(0, -window_height/2+cube_size/1.5, 0);
    glutSolidCube(cube_size);
}

void drawPlatform(void)
{
    glTranslatef(0, -window_height/2+10, 0);
    glScalef(window_width/10, 1, 1);
    glRotatef(25, 1, 0, 0);
    glColor3f(0, 1, 1);
    glutSolidCube(15);
}

void movingPlatforms(void)
{
    glPushMatrix();
        platform1_width = rand_width1;
        glTranslatef(30, -window_height/2 + 110, 0);
        glScalef(platform1_width, 1, 1);
        glRotatef(30, 1, 0, 0);
        glColor3f(0, 0.5, 0.7);
        glutSolidCube(17);
    glPopMatrix();

    glPushMatrix();
        platform2_width = rand_width2;
        glTranslatef(-30, -window_height/2 + 220, 0);
        glScalef(platform2_width, 1, 1);
        glRotatef(30, 1, 0, 0);
        glColor3f(0, 0.5, 0.7);
        glutSolidCube(17);
    glPopMatrix();

    glPushMatrix();
        platform3_width = rand_width3;
        glTranslatef(30, -window_height/2 + 340, 0);
        glScalef(platform3_width, 1, 1);
        glRotatef(30, 1, 0, 0);
        glColor3f(0, 0.5, 0.7);
        glutSolidCube(17);
    glPopMatrix();

    glPushMatrix();
        platform4_width = rand_width4;
        glTranslatef(0, -window_height/2 + 460, 0);
        glScalef(platform4_width, 1, 1);
        glRotatef(30, 1, 0, 0);
        glColor3f(0, 0.5, 0.7);
        glutSolidCube(17);
    glPopMatrix();

    glPushMatrix();
        platform5_width = rand_width5;
        glTranslatef(0, -window_height/2 + 580, 0);
        glScalef(platform5_width, 1, 1);
        glRotatef(30, 1, 0, 0);
        glColor3f(0, 0.5, 0.7);
        glutSolidCube(17);
    glPopMatrix();

    glPushMatrix();
        platform6_width = rand_width6;
        glTranslatef(0, -window_height/2 + 700, 0);
        glScalef(platform6_width, 1, 1);
        glRotatef(30, 1, 0, 0);
        glColor3f(0, 0.5, 0.7);
        glutSolidCube(17);
    glPopMatrix();

}

void move(void)
{
    if(key_pressed['a']) {
        if((translate_x - move_x > -window_width/2+cube_size/2)){
            translate_x -= move_x;
        }
    } else if(key_pressed['d']){
        if((translate_x + move_x < window_width/2-cube_size/2)){
            translate_x += move_x;
        }
    }
}
