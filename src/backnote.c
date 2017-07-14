#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "render.h"


struct bn_config conf = {0};
config_t cfg;
char *config_name;
struct bn_render render = {0};

bool
init()
{
    config_name = get_config_name();
   
    if(!strcmp(config_name, ""))
    {
        fprintf(stderr, "Configuration file not found\n");
        return false;
    } 
    if (load_config(config_name, &cfg, &conf))
    {
        config_destroy(&cfg);
        return false;
    }
    if(!validate_config(&conf))
    {
        fprintf(stderr, "Configuration file is not valid\nExiting\n");
        return false;
    }
    if(!init_render(&render))
        return false;
    render_wallpaper(&render, &conf);
    for(int i = 0; i < conf.notes_count; i++)
    {
        load_note_text(&conf.notes[i]);
        prepare_note(&render, &conf.notes[i]);
        render_note(&render, &conf.notes[i]);
    }
    update_root_bg(&render);
    return true;
}

void
clean_up()
{
    free(config_name);
    for(int i = 0; i < conf.notes_count; i++)
    {
        free(conf.notes[i].text);
    }
    clean_render(&render);
}

int main(int argc, char * argv[])
{
    if(!init())
        return 1;

    clean_up();
    return 0;
}
