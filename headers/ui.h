#ifndef __UI_C_
#define __UI_C_

#include <raylib.h>
#include <stddef.h>
#include <animation.h>

typedef struct {
    const char **items;
    size_t count;
    size_t capacity;
} Texture_Paths;

typedef struct {
    Font font;

    const char *output_path;
    Texture_Paths texture_paths;
    const char *import_Path;


    Animations animations;
} Ui_State;


void ui_update(void);
void ui_init(void);
void ui_unload(void);

#endif// __UI_C_
