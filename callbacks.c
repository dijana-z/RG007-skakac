#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "callbacks.h"

#define SPACE 32
#define ESC 27

/* camera position parameters */
static float cam_position_x = 0;
static float cam_position_y = 0;
static float cam_position_z = 10;

/* jumping parameters */
static float translate_y = 0.1;
static float move_y = 4;

/* size of the player */
static float cube_size = 40;

/* gravity parameter */
static float gravity = 0.4;

/* rotation parameter */
static float param = 10;

/* static window width and height sizes */
static float window_width;
static float window_height;

/* animation parameters */
static int start_animation = 0;
static int jump_up = 0;

/* light parameters */
GLfloat light_position[] = { 1, 1, 1, 0 };
GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1 };
GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };
GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };
GLfloat ambient_coeffs[] = { 1.0, 0.1, 0.1, 1 };
GLfloat diffuse_coeffs[] = { 0.0, 0.0, 0.8, 1 };
GLfloat specular_coeffs[] = { 1, 1, 1, 1 };
GLfloat shininess = 40;

void on_keyboard(unsigned char key, int x, int y)
{
  (void)x;
  (void)y;

  switch (key) {
    /* exit the game */
    case ESC:
      exit(0);
      break;
    /* start the game */
    case SPACE:
      if(!start_animation){
        start_animation = 1;
        glutTimerFunc(10, on_timer, 0);
      }
      break;
    /* jump up */
    case 'W':
    case 'w':
      if(start_animation){
        if(!jump_up){
        jump_up = 1;
        jumpUp();
        }
      }
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
  if(value != 0){
    return;
  }
  glutPostRedisplay();

  if(jump_up) {
    jumpUp();
  }
  if(start_animation){
    start_animation = 1;
    glutTimerFunc(10, on_timer, 0);
  }
}

void on_display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();
  gluLookAt(
    cam_position_x, cam_position_y, cam_position_z,
    0, 0, 0,
    0, 1, 0
  );

  initLighting();

  glTranslatef(0, translate_y, 0);
  glRotatef(param, 0, 1, 0);

  glPushMatrix();
  drawPlayer();
  glPopMatrix();

  glutSwapBuffers();
}

void initLighting(void)
{
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
  glutSolidCube(40);
}

void jumpUp(void)
{
  /* change the rotation parameter */
  param += 5%360;

  printf("%f\n", translate_y);
  if(translate_y > 50){
    move_y = -move_y;
    translate_y += move_y - gravity;
    move_y -= 1;
  }
  if(translate_y < 50){
    translate_y += move_y - gravity;
    move_y += 1;
  }

  if(translate_y <= 0){
    jump_up = 0;
    move_y = 4;
    translate_y = 0.1;
  }

  if(start_animation){
    start_animation = 1;
    glutTimerFunc(10, on_timer, 0);
  }
}
