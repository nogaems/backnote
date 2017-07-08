#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"

struct bn_config conf = {0};

int main(int argc, char * argv[])
{
    config_t cfg;
    char *name;
    name = get_config_name();   
   
    if(!strcmp(name, ""))
    {
        fprintf(stderr, "Configuration file not found\n");
        return 1;
    } 
    if (load_config(name, &cfg, &conf))
    {
        config_destroy(&cfg);
        return 1;
    }
    print_config(name);
    free(name);
    return 0;
}
