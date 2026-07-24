#ifndef __CANVAS_H_
#define __CANVAS_H_

#include <raylib.h>

typedef enum {
    PREVIEW,
    EDIT_FRAMES
} CANVAS_STATE;

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
    const char *output_path;
    const char *texture_Path;
} Canvas;

void Init_Canvas(Canvas *c);
void Draw_Canvas(Rectangle bondry, Canvas *c);

#endif// __CANVAS_H_
