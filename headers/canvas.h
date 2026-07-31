#ifndef __CANVAS_H_
#define __CANVAS_H_

#include <ui.h>
#include <raylib.h>

typedef enum {
    PREVIEW,
    EDIT_FRAMES
} CANVAS_STATE;

typedef struct {
    Texture2D *items;
    size_t count;
    size_t capacity;
} Textures;

typedef struct {
    const char* *items;
    size_t count;
    size_t capacity;
} Textures_Visited;

typedef struct {
    Texture2D *texture;
    Textures textures;
    Textures_Visited textures_visited;
    Shader backgroundShader;

    int gridSizeLocation;
    int canvasSizeLocation;

    CANVAS_STATE state;

    float zoom;
    Vector2 canvasPos;

    Rectangle edited_frame;
    Rectangle selected_frame;
    bool firstclick;
    Vector2 FirstClickPos;
    Vector2 SecondClickPos;
    bool Select_Mode;
    bool drawSelectedFrameMode;

} Canvas;


void Init_Canvas(Canvas *c, Ui_State *ui);
void Draw_Canvas(Ui_State *ui, Rectangle bondry, Canvas *c);
void load_textures(Canvas *c, Ui_State *ui);
void Unload_Textures(Canvas *c);

#endif// __CANVAS_H_
