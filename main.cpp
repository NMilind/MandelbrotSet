#include <stdio.h>
#include <stdlib.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

const int FPS = 10;
const int WIDTH = 2500;
const int HEIGHT = 1667;
const int MAX_N = 50;

enum KEYS {
    KEY_ESCAPE
};
bool key[1] = { false };
bool redraw = true;
bool doexit = false;
int n = 0;

double gridX[WIDTH][HEIGHT];
double gridY[WIDTH][HEIGHT];
bool gridFixed[WIDTH][HEIGHT];
ALLEGRO_COLOR gridColors[WIDTH][HEIGHT];

ALLEGRO_DISPLAY         *display = NULL;
ALLEGRO_EVENT_QUEUE     *event_queue = NULL;
ALLEGRO_TIMER           *timer = NULL;

ALLEGRO_COLOR lerp(double iter) {

    double hue = 270 - ((iter / (double)MAX_N) * 240);
    return al_color_hsv(hue, 1, 1);
}

bool initialize() {

    if (!al_init())
    {
        fprintf(stderr, "Failed to initialize allegro!\n");
        return false;
    }

    if (!al_install_keyboard())
    {
        fprintf(stderr, "Failed to initialized the keyboard!\n");
        return false;
    }

    timer = al_create_timer(1.0 / FPS);
    if (!timer)
    {
        fprintf(stderr, "Failed to create timer!\n");
        return false;
    }

    al_init_image_addon();
    al_init_primitives_addon();

    display = al_create_display(WIDTH, HEIGHT);
    if (!display)
    {
        fprintf(stderr, "Failed to create display!\n");
        al_destroy_timer(timer);
        return false;
    }

    event_queue = al_create_event_queue();
    if (!event_queue)
    {
        fprintf(stderr, "Failed to create event queue!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return false;
    }

    al_register_event_source(event_queue, al_get_display_event_source(display));

    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    al_register_event_source(event_queue, al_get_keyboard_event_source());

    for (int c = 0; c < WIDTH; c++)
    {
        for (int r = 0; r < HEIGHT; r++)
        {
            gridX[c][r] = 0.0;
            gridY[c][r] = 0.0;
            gridFixed[c][r] = false;
        }
    }

    return true;
}

void createSet() {

    while (n <= MAX_N) {
        for (int c = 0; c < WIDTH; c++) {
            for (int r = 0; r < HEIGHT; r++) {
                if (!gridFixed[c][r]) {
                    double x = ((double) c - (2.0 * WIDTH / 3.0)) * (3.0 / WIDTH);
                    double y = ((double) r - (HEIGHT / 2.0)) * (2.0 / HEIGHT);

                    double xtemp = (gridX[c][r] * gridX[c][r]) - (gridY[c][r] * gridY[c][r]) + x;
                    gridY[c][r] = 2 * gridX[c][r] * gridY[c][r] + y;
                    gridX[c][r] = xtemp;

                    double val = (gridX[c][r] * gridX[c][r]) + (gridY[c][r] * gridY[c][r]);
                    if (val > 4) {
                        gridColors[c][r] = lerp(n);
                        gridFixed[c][r] = true;
                    } else {
                        gridColors[c][r] = al_map_rgb(0, 0, 0); //lerp(0);
                    }
                }
            }
        }

        n += 1;
    }
}

void mainLoop() {

    if (key[KEY_ESCAPE]) {
        doexit = true;
    }

    redraw = true;
}

void draw() {

    for (int c = 0; c < WIDTH; c++) {
        for (int r = 0; r < HEIGHT; r++) {
            al_draw_filled_rectangle(c, r, c+1, r+1, gridColors[c][r]);
        }
    }
}

int main(int argc, char **argv) {

    if (!initialize()) {
        return -1;
    }

    createSet();

    al_clear_to_color(al_map_rgb(0, 0, 0));

    al_flip_display();

    al_start_timer(timer);

    while (!doexit) {

        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            mainLoop();
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            doexit = true;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (ev.keyboard.keycode) {
                case ALLEGRO_KEY_ESCAPE:
                    key[KEY_ESCAPE] = true;
                    break;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            switch (ev.keyboard.keycode) {
                case ALLEGRO_KEY_ESCAPE:
                    key[KEY_ESCAPE] = false;
                    break;
            }
        }

        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;
            al_clear_to_color(al_map_rgb(0, 0, 0));
            draw();
            al_flip_display();
        }
    }

    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    return 0;
}