#include <stdio.h>
#include <stdlib.h>
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
