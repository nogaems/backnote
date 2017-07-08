#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "config.h"
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

bool
check_file_reads(const char * name)
{
    return access(name, R_OK) != -1 ? true : false;
}

char *
get_config_name()
{
    char *result = malloc(sizeof(char *));
    memset(result, 0, 0);
    char possible_path[NUMBER_OF_PATHS][MAX_LENGTH_OF_PATH] =
    {
        "./backnote.cfg", "~/.backnote.cfg", "/etc/backnote.cf"
    };
    for(int i = 0; i < NUMBER_OF_PATHS; i++)
    {
        if(check_file_reads(possible_path[i]))
        {
            strcpy(result, possible_path[i]);
            return result;
        }
    }        
    return result;
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
        fprintf(stderr, "Syntax error in the configuration file \"%s\"\n", name);
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
    printf("config file name: \"%s\"\n\n", name);
    printf("wallpaper: %s\n", conf.wallpaper);
    printf("daemon: %i\n\n", conf.daemon);   
    printf("notes count: %i\n", conf.notes_count);
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

bool
validate_config(struct bn_config *conf)
{
    Display *d;
    int s;
    int wpx,hpx;
    bool result = true;
    if (!(d = XOpenDisplay(NULL)))
    {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }
    s = DefaultScreen(d);
    wpx = XDisplayWidth(d, s);
    hpx = XDisplayHeight(d, s);

    if(!check_file_reads(conf->wallpaper))
        FALSE_CHECK_WITH_MESSAGE(conf->wallpaper, "Unable to read the file");
    if(conf->notes_count == 0)
        FALSE_CHECK_WITH_MESSAGE("amount of notes", "you should add at least one note");
    if(conf->notes_count >= MAX_NOTES)
        FALSE_CHECK_WITH_MESSAGE("amount of notes",
                                 STR_CONC_INT("too many notes, it should be lesser or equals to ",
                                              MAX_NOTES));
    for(int i = 0; i < conf->notes_count; i++)
    {
        struct bn_note note = conf->notes[i];        

        if(!check_file_reads(note.file))
            FALSE_CHECK_WITH_MESSAGE(note.file, "Unable to read the file");
        
        if(note.position.x1 > wpx)
            FALSE_CHECK_WITH_MESSAGE("x1", "This value should be lesser than you display width");
        if(note.position.y1 > hpx)
            FALSE_CHECK_WITH_MESSAGE("y1", "This value should be lesser than you display height");
        if(note.position.x2 < note.position.x1 && note.position.x2 != 0)
            FALSE_CHECK_WITH_MESSAGE("x2", "This value should be bigger that x1");
        if(note.position.y2 < note.position.y1 && note.position.y2 != 0)
            FALSE_CHECK_WITH_MESSAGE("y2", "This value should be bigger that y1");

        FcConfig *config = FcInitLoadConfigAndFonts();
        FcResult r;
        /* 
           Configure the search pattern, 
           assume "name" is desired font name 
        */
        FcPattern* pat = FcNameParse((const FcChar8*)(note.style.font));
        FcConfigSubstitute(config, pat, FcMatchPattern);
        FcDefaultSubstitute(pat);
        /* Find the font */
        FcPattern* font = FcFontMatch(config, pat, &r);
        if (font)
        {
            FcChar8* file = NULL;
            if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch)
            {
                /* 
                   I don't actually know is this a common behavior
                   of fontconfig or it's just to me, but we need to
                   compare the font name that has been found and the
                   name that was specified in the configuration file.
                   It's because fontconfig can return some *default*
                   font if specified font cound not be found (this 
                   exactly happens to me).
                */
                if(strstr((char*)file, note.style.font) == NULL)
                    FALSE_CHECK_WITH_MESSAGE(note.style.font, "Could not find this font");
            }
            FcPatternDestroy(font);
        }
        FcPatternDestroy(pat);

        if(note.style.size < MIN_FONT_SIZE || note.style.size > MAX_FONT_SIZE)
            FALSE_CHECK_WITH_MESSAGE("size",
                                     STR_CONC_INT("This value should be in range from ",
                                                  MIN_FONT_SIZE)
                                     STR_CONC_INT(" to ",
                                                  MAX_FONT_SIZE));
        if(note.style.color < 0 || note.style.color > 255)
            FALSE_CHECK_WITH_MESSAGE("color", "This value should be in range from 0 to 255");
        if(note.style.border_color < 0 || note.style.border_color > 255)
            FALSE_CHECK_WITH_MESSAGE("border_color", "This value should be in range from 0 to 255");
        if(note.style.border_thickness < 0 || note.style.border_thickness > MAX_BORDER_THICKNESS)
            FALSE_CHECK_WITH_MESSAGE("border_thickness",
                                     STR_CONC_INT("This value should be in range from 1 to ",
                                                  MAX_BORDER_THICKNESS));
        if(note.style.bg_color < -1 || note.style.bg_color > 255)
            FALSE_CHECK_WITH_MESSAGE("bg_color",
                                     "This value should be in range from -1 to 255 (-1 is transparent, 0 is black)");
    }
    return result;
}
