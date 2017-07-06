#pragma once
#include <stdbool.h>
#include <libconfig.h>

extern config_t cfg;

bool check_config_exists(const char * name);
config_t load_config(char * name);
bool validate_config(config_t config);
