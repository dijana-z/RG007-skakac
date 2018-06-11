#include <stdlib.h>
#include <time.h>
#include <GL/glut.h>
#include "callbacks.h"
#include "functions.h"

extern float window_width, window_height;

int main(int argc, char** argv)
{
  /* initializing glut and window */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition(200, 50);
    glutCreateWindow("Jumping Cube");

    /* callback functions */
    glutDisplayFunc(on_display);
    glutKeyboardFunc(on_keyboard);
    glutKeyboardUpFunc(on_key_release);
    glutReshapeFunc(on_reshape);

    /* hiding the cursor */
    glutSetCursor(GLUT_CURSOR_NONE);

    /* clear window */
    glClearColor(0, 0, 0, 0);

    srand(time(NULL));

    /* disabling the key repeat */
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

    /* init lighting parameters */
    init_lighting();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    /* set the initial coordinates */
    init_coordinates();

    /* set the textures */
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    initialize_texture();

    /* start the main loop */
    glutMainLoop();

    return 0;
}
