#include <GL/glut.h>
#include "functions.h"

#define MAX_KEYS 256

extern float window_width, window_height;
extern float cube_size;
extern float translate_x, move_x;
extern int key_pressed[MAX_KEYS];


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

void drawPlayer(void)
{
    glColor3f(1, 0, 0);
    glTranslatef(0, -window_height/2+cube_size/1.5, 0);
    glutSolidCube(cube_size);
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
