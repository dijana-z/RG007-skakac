#pragma once

typedef struct {
    float size;
    float x_position;
    float y_position;
    int pot_base;
    int ground;
} Player;

typedef struct {
    float width;
    float scale_param;
    float x_position;
    float y_position;
    float move;
    int has_coin;
    int pl_no;
} Platform;

typedef struct {
    float x_position;
    float y_position;
    float start_y;
    int pl_no;
} Coin;

typedef struct {
    float y_top;
    float y_bottom;
    float x_left;
    float x_right;
} Box;

void on_keyboard(unsigned char key, int x, int y);
void on_key_release(unsigned char key, int x, int y);

void on_display(void);
void on_reshape(int width, int height);

void on_timer(int value);
