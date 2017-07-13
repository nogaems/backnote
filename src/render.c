#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include "render.h"

void
load_note_text(struct bn_note *note)
{
    FILE *f = fopen(note->file, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  

    note->text = malloc(fsize + 1);
    fread(note->text, fsize, 1, f);
    fclose(f);
    note->text[fsize] = 0;
}

bool
init_render(struct bn_render *render)
{
    if (!(render->dpy = XOpenDisplay(NULL)))
    {
        fprintf(stderr, "Cannot open display\n");
        return false;
    }
    render->screen = DefaultScreen(render->dpy);
    render->width = XDisplayWidth(render->dpy, render->screen);
    render->height = XDisplayHeight(render->dpy, render->screen);

    render->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                 render->width, render->height);
    render->cr = cairo_create(render->surface);
    return true;
}

void
clean_render(struct bn_render *render)
{
    cairo_destroy (render->cr);
    cairo_surface_destroy (render->surface);
}

bool check_fit(struct bn_render *render, struct bn_note *note, char *text)
{
    cairo_text_extents_t extents;

    cairo_select_font_face(render->cr, note->style.font, 0,0);
    cairo_set_font_size(render->cr, note->style.size);
    cairo_text_extents(render->cr, text, &extents);

    if(note->w >= (extents.width + note->style.border_thickness * 2 ) &&
       note->h >= (extents.height + note->style.border_thickness * 2))
        return true;
    return false;
}

void prepare_note(struct bn_render *render, struct bn_note *note)
{
    if(note->position.x2 != 0)
    {
        note->w = note->position.x2 - note->position.x1;
    } else {
        note->w = render->width - note->position.x1;
    }
    if(note->position.y2 != 0)
    {
        note->h = note->position.y2 - note->position.y1;
    } else {
        note->h = render->height - note->position.y1;
    }

    cairo_font_extents_t fe;
    cairo_select_font_face(render->cr, note->style.font, 0,0);
    cairo_set_font_size(render->cr, note->style.size);
    cairo_font_extents(render->cr, &fe);
    int max_lbi_counter = note->h / fe.height + 1;

    note->lbi = malloc(sizeof(int)*max_lbi_counter);
    note->lbi_counter = 0;

    char *buf;
    int max_buf_len = (int)(note->w / fe.max_x_advance + 1);
    buf = malloc(sizeof(char) * max_buf_len);
    memset(buf, '\0', max_buf_len);

    int text_len = strlen(note->text);
    int space = 0;

    for (int i = 0; i < text_len; i++)
    {
        if(IS_THIS("\n"))
        {
            note->lbi[note->lbi_counter] = i;
            note->lbi_counter++;
            space = i;
            memset(buf, 0, max_buf_len);
            continue;
        }
        strncat(buf, note->text + (sizeof(char) * i), 1);
        if(IS_THIS(" ") && i > space)
        {
            space = i;
        }
        if(check_fit(render, note, buf))
        {
            continue;
        } else {            
            if(space == 0 || space == note->lbi[note->lbi_counter-1])
            {
                /* TODO: Crop this word to the maximum width thus it would fit in the note width */
                fprintf(stderr, "Warning: word \"%s\" is wider than note area width!\n", buf);
                note->lbi[note->lbi_counter] = i;
            } else {
                note->lbi[note->lbi_counter] = space;
                i = space;
            }
            note->lbi_counter++;
            memset(buf, 0, max_buf_len);
        }
    }
}

void
render_wallpaper(struct bn_render *render, struct bn_config *conf)
{
    cairo_surface_t *wallpaper =
        cairo_image_surface_create_from_png(conf->wallpaper);

    cairo_set_source_rgba(render->cr, 0,0,0, 1);
    cairo_paint(render->cr);

    cairo_set_source_surface(render->cr, wallpaper, 0, 0);
    cairo_paint(render->cr);

    cairo_surface_flush(render->surface);
}

void
render_note(struct bn_render *render, struct bn_note *note)
{
    cairo_font_extents_t fe;

    cairo_select_font_face(render->cr, note->style.font, 0,0);
    cairo_set_font_size(render->cr, note->style.size);
    cairo_font_extents(render->cr, &fe);

    /* Backgroung */
    /* TODO: add a level of transparency in the config */
    double bg_alpha = (note->style.bg_color + 1) ? 1.0 : 0.0;
    double bg_color = bg_alpha ? (double)note->style.bg_color / 255 : 0;
    cairo_set_source_rgba(render->cr, GRAYSCALE(bg_color), bg_alpha);
    cairo_rectangle(render->cr,
                    (double)note->position.x1,
                    (double)note->position.y1,
                    (double)note->w,
                    (double)note->h);
    cairo_fill(render->cr);

    /* Border */
    double border;
    border = note->style.border ? (double)note->style.border_thickness : 0;
    cairo_set_line_width(render->cr, border);
    cairo_set_source_rgb(render->cr, GRAYSCALE((double)note->style.border_color/255));
    cairo_rectangle(render->cr,
                    (double)note->position.x1,
                    (double)note->position.y1,
                    (double)note->w,
                    (double)note->h);
    cairo_stroke(render->cr);

    /* Text */
    cairo_set_source_rgb(render->cr, GRAYSCALE((double)note->style.color/255));

    double x, y;
    x = (double)note->position.x1 + border;
    y = (double)note->position.y1 + border;
    int max_buf_len = sizeof(char) * (int)(note->w/fe.max_x_advance) + 2;
    char *buf = malloc(max_buf_len);
    int offset;
    int lbi_prev = 0;

    for(int i = 0; i < note->lbi_counter; i++)
    {
        y += fe.height;
        cairo_move_to(render->cr, x, y);
        memset(buf, '\0', max_buf_len);
        strncpy(buf,
                note->text + sizeof(char)*lbi_prev,
                note->lbi[i] - lbi_prev);
        buf[note->lbi[i]-lbi_prev] = '\0';
        offset = (!strncmp(buf, " ", 1) || !strncmp(buf, "\n", 1)) ? 1 : 0;
        lbi_prev = note->lbi[i];
        cairo_show_text(render->cr, buf+offset);
    }
}

void
print_prepared_text(struct bn_note *note)
{
    char *buf;
    int offset;
    int lbi_prev = 0;
    buf = malloc(1024*sizeof(char));
    for(int i = 0; i < note->lbi_counter; i++)
    {
        buf = memset(buf, 0, 1024);
        offset = lbi_prev ? 1 : 0;
        strncpy(buf,
                note->text + (lbi_prev*sizeof(char)) + offset,
                note->lbi[i]-lbi_prev - offset);
        buf[note->lbi[i]-lbi_prev] = '\0';
        lbi_prev = note->lbi[i];
        printf("line %i (lbi:%i): \"%s\"\n", i, note->lbi[i], buf);
    }
    free(buf);
}

