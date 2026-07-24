#ifndef __UI_C_
#define __UI_C_

#include <raylib.h>

typedef struct {
    Font font;
} Ui_State;

void ui_update(void);
void ui_init(void);
void ui_unload(void);

#endif// __UI_C_
