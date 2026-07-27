#ifndef __CANVAS_H_
#define __CANVAS_H_

#include <ui.h>
#include <raylib.h>

typedef enum {
    PREVIEW,
    EDIT_FRAMES
} CANVAS_STATE;

// TODO: It sorta of becoming a State struct
// we should seprate all the input/output paths into a deffrent struct
// maybe we should make the `ui.h` hold the struct.
typedef struct {
    Texture2D texture;
    Shader backgroundShader;

    int gridSizeLocation;
    int canvasSizeLocation;

    CANVAS_STATE state;

    float zoom;
    Vector2 canvasPos;

    Rectangle Selecte_Box;
    bool firstclick;
    Vector2 FirstClickPos;
    Vector2 SecondClickPos;
    bool Select_Mode;

} Canvas;

void Init_Canvas(Canvas *c, Ui_State *ui);
void Draw_Canvas(Rectangle bondry, Canvas *c);

#endif// __CANVAS_H_
