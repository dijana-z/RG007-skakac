#include <stdlib.h>
#include <GL/glut.h>
#include "callbacks.h"

int main(int argc, char** argv)
{
  /* initializing window */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

  glutInitWindowSize(800, 800);
  glutInitWindowPosition(200, 50);
  glutCreateWindow("Jumping Cube");

  /* callback functions */
  glutDisplayFunc(on_display);
  glutKeyboardFunc(on_keyboard);
  glutReshapeFunc(on_reshape);
  
  /* clear window */
  glClearColor(0, 0, 0, 0);

  glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

  /* start the main loop */
	glutMainLoop();

	return 0;
}
