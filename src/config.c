#include <unistd.h>
#include "config.h"

config_t cfg;


bool
check_config_exists(const char * name)
{
    return access(name, R_OK) != -1 ? true : false;
}



