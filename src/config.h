#pragma once
#include <stdbool.h>
#include <libconfig.h>

#define MAX_NOTES 8
#define NUMBER_OF_PATHS 3
#define MAX_LENGTH_OF_PATH 128

#define MAX_FONT_SIZE 32
#define MIN_FONT_SIZE 8
#define MAX_BORDER_THICKNESS 8

extern struct bn_config conf;

/* 
   setting - config_t *, 
   name - char *, 
   destination - pointer, 
   required - 1 or 0 
*/
#define LOAD_INT(s, n, d, r) \
    if(!config_setting_lookup_int(s, n, d) && r)  \
    { \
        fprintf(stderr, "Configuration error. Please, set \"%s\" parameter.\n", n); \
        return -1; \
    }
#define LOAD_STR(s, n, d, r) \
    if(!config_setting_lookup_string(s, n, d) && r) \
    { \
        fprintf(stderr, "Configuration error. Please, set \"%s\" parameter.\n", n); \
        return -1; \
    }
#define LOAD_BOOL(s, n, d, r) \
    if(!config_setting_lookup_bool(s, n, d) && r)  \
    { \
        fprintf(stderr, "Configuration error. Please, set \"%s\" parameter.\n", n); \
        return -1; \
    }

/* 
   Used in validate_config function
   o - object of validation
   e - explanation of a problem
*/
#define FALSE_CHECK_WITH_MESSAGE(o, e)                                  \
    {                                                                   \
        fprintf(stderr, "[config validation error] %s: %s\n", o, e);    \
        result = false;                                                 \
    }
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define STR_CONC_INT(s1, s2) s1 STR(s2)

/* Note style */
struct bn_note_style {
    const char *font;
    int size;
    int color;
    int border;
    int border_thickness;
    int border_color;
    int bg_color;
};

/* Note position */
struct bn_note_position {
    int x1, x2, y1, y2;
    int fit;
};

/* Note structure */
struct bn_note {
    const char *file;
    struct bn_note_position position;
    struct bn_note_style style;
};

/* Processed config */
struct bn_config {
    const char *wallpaper;
    int daemon;
    struct bn_note notes[MAX_NOTES];
    unsigned int notes_count;
};

char *get_config_name();
bool check_file_reads(const char *name);
int load_config(char *name, config_t *cfg, struct bn_config *conf);
void print_config(char *name);
bool validate_config(struct bn_config *conf);
