#pragma once
#include <stdbool.h>
#include <cairo/cairo.h>
#include <X11/Xlib.h>

#include "config.h"

#define IS_THIS(x)                                      \
    !strncmp(note->text + (sizeof(char) * i), x, 1)     \

#define GRAYSCALE(x) x, x, x

#define DUMP ".backnote.png"

struct bn_render {
    cairo_t *cr;
    cairo_surface_t *surface;
    Display *dpy;
    int screen;
    int height;
    int width;
};

void load_note_text(struct bn_note *note);
bool init_render(struct bn_render *render);
void clean_render(struct bn_render *render);
void prepare_note(struct bn_render *render, struct bn_note *note);
bool check_fit(struct bn_render *render, struct bn_note *note, char *text);
void render_wallpaper(struct bn_render *render, struct bn_config *conf);
void render_note(struct bn_render *render, struct bn_note *note);
void print_prepared_text(struct bn_note *note);
void update_root_bg(struct bn_render *render);
