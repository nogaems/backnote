#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "config.h"

bool
check_config_exists(const char * name)
{
    return access(name, R_OK) != -1 ? true : false;
}

int
load_config(char *name, config_t *cfg, struct bn_config *conf)
/* 
   cfg is a pointer to libconfig structure
   conf is a pointer to the internal configuration structure
*/
{
    config_setting_t *setting;    
    config_init(cfg);    
    if(!config_read_file(cfg, name))
    {
        fprintf(stderr, "Can't read configuration file \"%s\"\n", name);
        return -1;
    }
    
    if(!config_lookup_string(cfg, "wallpaper", &conf->wallpaper))
    {
        fprintf(stderr, "Configuration error. Please, set \"wallpaper\" parameter.\n");
        return -1;
    }
    if(!config_lookup_bool(cfg, "daemon", &conf->daemon))
    {
        fprintf(stderr, "Configuration error. Please, set \"daemon\" parameter.\n");
        return -1; 
    }
    
    if (!(setting = config_lookup(cfg, "notes")))
    {
        fprintf(stderr, "Section \"notes\" is missing.\n");  
        return -1;
    }
    
    conf->notes_count = config_setting_length(setting);
    for (int i = 0; i < conf->notes_count; i++)
    {        
        config_setting_t *note = config_setting_get_elem(setting, i);
        LOAD_STR(note, "file", &conf->notes[i].file, 1);
        
        config_setting_t *position = config_setting_lookup(note, "position");
        LOAD_INT(position, "x1", &conf->notes[i].position.x1, 1);
        LOAD_INT(position, "y1", &conf->notes[i].position.y1, 1);
        LOAD_INT(position, "x2", &conf->notes[i].position.x2, 0);
        LOAD_INT(position, "y2", &conf->notes[i].position.y2, 0);
        LOAD_BOOL(position, "fit", &conf->notes[i].position.fit, 0);

        config_setting_t *style = config_setting_lookup(note, "style");
        LOAD_STR(style, "font", &conf->notes[i].style.font, 1);
        LOAD_INT(style, "size", &conf->notes[i].style.size, 1);
        LOAD_INT(style, "color", &conf->notes[i].style.color, 0);
        LOAD_BOOL(style, "border", &conf->notes[i].style.border, 0);
        LOAD_INT(style, "border_color", &conf->notes[i].style.border_color, 0);
        LOAD_INT(style, "border_thickness", &conf->notes[i].style.border_thickness, 0);
        LOAD_INT(style, "bg_color", &conf->notes[i].style.bg_color, 0);        
    }
    return 0;
}

void
print_config(char *name)
{
    printf("wallpaper: %s\n", conf.wallpaper);
    printf("daemon: %i\n", conf.daemon);   
    printf("----------\nnotes count: %i\n", conf.notes_count);
    for (int i = 0; i < conf.notes_count; i++)
    {
        printf("note #%i:\n", i);
        printf("note file: %s\n", conf.notes[0].file);
        printf("x1: %i\n", conf.notes[0].position.x1);
        printf("y1: %i\n", conf.notes[0].position.y1);
        printf("x2: %i\n", conf.notes[0].position.x2);
        printf("y2: %i\n", conf.notes[0].position.y2);
        printf("fit: %i\n", conf.notes[0].position.fit);
        printf("font: %s\n", conf.notes[0].style.font);
        printf("size: %i\n", conf.notes[0].style.size);
        printf("color: %i\n", conf.notes[0].style.color);
        printf("border: %i\n", conf.notes[0].style.border);
        printf("border_color: %i\n", conf.notes[0].style.border_color);
        printf("border_thickness: %i\n", conf.notes[0].style.border_thickness);
        printf("bg_color: %i\n", conf.notes[0].style.bg_color);
    }
}
