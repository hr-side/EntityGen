#ifndef __UI_C_
#define __UI_C_

#include <raylib.h>
#include <stddef.h>
#include <animation.h>

typedef enum {
    WELLCOME_WINDOW,
    ANIMATION_WINDOW,
    COLLISION_WINDOW,
    WINDOW_COUNT
} WINDOW_State;

typedef struct {
    const char **items;
    size_t count;
    size_t capacity;
} Texture_Paths;

typedef struct {
    Font font;

    Texture_Paths texture_paths;
    size_t current_texture_index;
    bool NeedsLoading;

    const char *output_path;
    const char *import_Path;

    size_t currentAnimationIndex;
    size_t currentVisiableFrameIndex;
    bool IsFrameSelected;
    bool IsAnimationSelected;
    Animations animations;

    WINDOW_State Win_state;
    bool ShowMenu;
} Ui_State;


void ui_update(void);
void ui_init(void);
void ui_unload(void);

#endif// __UI_C_
