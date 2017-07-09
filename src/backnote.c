#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "render.h"

struct bn_config conf = {0};
config_t cfg;
char *config_name;

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
    for(int i = 0; i < conf.notes_count; i++)
    {
        load_note_text(&conf.notes[i]);
    }
    return true;
}

void
clean_up()
{
    free(config_name);
    for(int i = i; i < conf.notes_count; i++)
    {
        free(conf.notes[i].text);
    }
}

int main(int argc, char * argv[])
{
    if(!init())
        return 1;
    return 0;
}
