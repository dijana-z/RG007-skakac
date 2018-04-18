#pragma once

void on_keyboard(unsigned char key, int x, int y);
void on_display(void);
void on_reshape(int width, int height);
void on_timer(int value);

void initLighting(void);
void drawPlayer(void);
void jumpUp(void);
